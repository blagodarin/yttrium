#include "obj.h"

#include <yttrium/exceptions.h>
#include <yttrium/math/vector.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/vertex_buffer.h>
#include <yttrium/storage/reader.h>

#include <regex>

#include <boost/algorithm/string/trim.hpp>
#include <boost/optional/optional.hpp>

namespace
{
	const std::string _float = R"(\s+([+-]?\d+(?:\.\d+)))";
	const std::string _index = R"(\s+(\S+))";

	const std::regex _obj_empty_regex(R"(\s*(?:#.*)?)", std::regex::optimize);
	const std::regex _obj_f_regex("f" + _index + _index + _index, std::regex::optimize);
	const std::regex _obj_v_regex("v" + _float + _float + _float, std::regex::optimize);
	const std::regex _obj_vn_regex("vn" + _float + _float + _float, std::regex::optimize);
	const std::regex _obj_vt_regex("vt" + _float + _float, std::regex::optimize);

	const std::regex _obj_f_v_regex(R"(([1-9]\d*))", std::regex::optimize);
	const std::regex _obj_f_vt_regex(R"(([1-9]\d*)/([1-9]\d*))", std::regex::optimize);
	const std::regex _obj_f_vn_regex(R"(([1-9]\d*)//([1-9]\d*))", std::regex::optimize);
	const std::regex _obj_f_vtn_regex(R"(([1-9]\d*)/([1-9]\d*)/([1-9]\d*))", std::regex::optimize);

	class ObjState
	{
	public:
		bool process_line(const std::string& line)
		{
			std::smatch match;
			if (std::regex_match(line, match, _obj_v_regex))
				_vertices.emplace_back(std::stod(match[1]), std::stod(match[2]), std::stod(match[3]));
			else if (std::regex_match(line, match, _obj_vt_regex))
				_texcoords.emplace_back(std::stod(match[1]), std::stod(match[2]));
			else if (std::regex_match(line, match, _obj_vn_regex))
				_normals.emplace_back(std::stod(match[1]), std::stod(match[2]), std::stod(match[3]));
			else if (std::regex_match(line, match, _obj_f_regex))
			{
				for (int i = 1; i <= 3; ++i)
				{
					const auto index = make_index(match[i]);
					if (!index || !process_index(*index))
						return false;
				}
			}
			else
				return false;
			return true;
		}

		bool create_buffers(Renderer& renderer, UniquePtr<VertexBuffer>& vertex_buffer, UniquePtr<IndexBuffer>& index_buffer)
		{
			switch (_face_format)
			{
			case FaceFormat::v:
				vertex_buffer = renderer.create_vertex_buffer({ VA::f4 }, _vertex_data.size() / (4 * sizeof(float)), _vertex_data.data());
				break;
			case FaceFormat::vt:
				vertex_buffer = renderer.create_vertex_buffer({ VA::f4, VA::f2 }, _vertex_data.size() / (6 * sizeof(float)), _vertex_data.data());
				break;
			case FaceFormat::vn:
				vertex_buffer = renderer.create_vertex_buffer({ VA::f4, VA::f4 }, _vertex_data.size() / (8 * sizeof(float)), _vertex_data.data());
				break;
			case FaceFormat::vtn:
				vertex_buffer = renderer.create_vertex_buffer({ VA::f4, VA::f2, VA::f4 }, _vertex_data.size() / (10 * sizeof(float)), _vertex_data.data());
				break;
			default:
				return false;
			}
			index_buffer = renderer.create_index_buffer(IndexFormat::U16, _index_data.size(), _index_data.data());
			return true;
		}

	private:
		enum class FaceFormat
		{
			unknown,
			v,
			vt,
			vn,
			vtn,
		};

		boost::optional<std::tuple<int, int, int>> make_index(const std::string& text)
		{
			std::smatch match;
			switch (_face_format)
			{
			case FaceFormat::v:
				if (std::regex_match(text, match, _obj_f_v_regex))
					return std::make_tuple(std::stoi(match[1]) - 1, -1, -1);
				break;
			case FaceFormat::vt:
				if (std::regex_match(text, match, _obj_f_vt_regex))
					return std::make_tuple(std::stoi(match[1]) - 1, std::stoi(match[2]) - 1, -1);
				break;
			case FaceFormat::vn:
				if (std::regex_match(text, match, _obj_f_vn_regex))
					return std::make_tuple(std::stoi(match[1]) - 1, -1, std::stoi(match[2]) - 1);
				break;
			case FaceFormat::vtn:
				if (std::regex_match(text, match, _obj_f_vtn_regex))
					return std::make_tuple(std::stoi(match[1]) - 1, std::stoi(match[2]) - 1, std::stoi(match[3]) - 1);
				break;
			default:
				if (std::regex_match(text, match, _obj_f_v_regex))
				{
					_face_format = FaceFormat::v;
					return std::make_tuple(std::stoi(match[1]) - 1, -1, -1);
				}
				else if (std::regex_match(text, match, _obj_f_vt_regex))
				{
					_face_format = FaceFormat::vt;
					return std::make_tuple(std::stoi(match[1]) - 1, std::stoi(match[2]) - 1, -1);
				}
				else if (std::regex_match(text, match, _obj_f_vn_regex))
				{
					_face_format = FaceFormat::vn;
					return std::make_tuple(std::stoi(match[1]) - 1, -1, std::stoi(match[2]) - 1);
				}
				else if (std::regex_match(text, match, _obj_f_vtn_regex))
				{
					_face_format = FaceFormat::vtn;
					return std::make_tuple(std::stoi(match[1]) - 1, std::stoi(match[2]) - 1, std::stoi(match[3]) - 1);
				}
			}
			return {};
		}

		bool process_index(const std::tuple<int, int, int>& index)
		{
			const auto index_v = std::get<0>(index);
			const auto index_t = std::get<1>(index);
			const auto index_n = std::get<2>(index);

			if (index_v >= static_cast<int>(_vertices.size())
				|| index_t >= static_cast<int>(_texcoords.size())
				|| index_n >= static_cast<int>(_normals.size()))
				return false;

			const auto i = std::find(_indices.cbegin(), _indices.cend(), index);
			if (i != _indices.cend())
			{
				_index_data.emplace_back(i - _indices.cbegin());
				return true;
			}

			BufferAppender<float> appender(_vertex_data);
			appender << _vertices[index_v].x << _vertices[index_v].y << _vertices[index_v].z << _vertices[index_v].w;
			if (index_t >= 0)
				appender << _texcoords[index_t].x << _texcoords[index_t].y;
			if (index_n >= 0)
				appender << _normals[index_n].x << _normals[index_n].y << _normals[index_n].z << _normals[index_n].w;

			_index_data.emplace_back(_indices.size());
			_indices.emplace_back(index);
			return true;
		}

	private:
		FaceFormat _face_format = FaceFormat::unknown;
		std::vector<Vector4> _vertices;
		std::vector<Vector2> _texcoords;
		std::vector<Vector4> _normals;
		std::vector<std::tuple<int, int, int>> _indices;
		Buffer _vertex_data;
		std::vector<uint16_t> _index_data;
	};
}

void load_obj_mesh(Reader&& reader, Renderer& renderer, UniquePtr<VertexBuffer>& vertex_buffer, UniquePtr<IndexBuffer>& index_buffer)
{
	std::string line;
	size_t line_number = 0;
	ObjState state;
	while (reader.read_line(line))
	{
		++line_number;
		if (std::regex_match(line, _obj_empty_regex))
			continue;
		boost::algorithm::trim(line);
		if (!state.process_line(line))
			throw DataError("OBJ processing error (", reader.name(), ":", line_number, ")");
	}
	if (!state.create_buffers(renderer, vertex_buffer, index_buffer))
		throw DataError("Bad OBJ");
}
