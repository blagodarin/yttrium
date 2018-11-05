#include "obj.h"

#include <yttrium/exceptions.h>
#include <yttrium/math/vector.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "../mesh_data.h"

#include <optional>
#include <regex>
#include <tuple>

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

	constexpr auto _no_index = std::numeric_limits<size_t>::max();

	class ObjState
	{
	public:
		bool process_line(const std::string& line, Yttrium::MeshData& data)
		{
			std::smatch match;
			if (std::regex_match(line, match, _obj_v_regex))
				_vertices.emplace_back(std::stof(match[1]), std::stof(match[2]), std::stof(match[3]));
			else if (std::regex_match(line, match, _obj_vt_regex))
				_texcoords.emplace_back(std::stof(match[1]), std::stof(match[2]));
			else if (std::regex_match(line, match, _obj_vn_regex))
				_normals.emplace_back(std::stof(match[1]), std::stof(match[2]), std::stof(match[3]));
			else if (std::regex_match(line, match, _obj_f_regex))
			{
				for (size_t i = 1; i <= 3; ++i)
				{
					const auto index = make_index(match[i]);
					if (!index || !process_index(*index, data))
						return false;
				}
			}
			else
				return false;
			return true;
		}

		bool finalize(Yttrium::MeshData& data)
		{
			using Yttrium::VA;

			switch (_face_format)
			{
			case FaceFormat::v:
				data._vertex_format = { VA::f3 };
				return true;
			case FaceFormat::vt:
				data._vertex_format = { VA::f3, VA::f2 };
				return true;
			case FaceFormat::vn:
				data._vertex_format = { VA::f3, VA::f3 };
				return true;
			case FaceFormat::vtn:
				data._vertex_format = { VA::f3, VA::f2, VA::f3 };
				return true;
			default:
				return false;
			}
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

		std::optional<std::tuple<size_t, size_t, size_t>> make_index(const std::string& text)
		{
			std::smatch match;
			switch (_face_format)
			{
			case FaceFormat::v:
				if (std::regex_match(text, match, _obj_f_v_regex))
					return std::make_tuple(std::stoul(match[1]) - 1, _no_index, _no_index);
				break;
			case FaceFormat::vt:
				if (std::regex_match(text, match, _obj_f_vt_regex))
					return std::make_tuple(std::stoul(match[1]) - 1, std::stoul(match[2]) - 1, _no_index);
				break;
			case FaceFormat::vn:
				if (std::regex_match(text, match, _obj_f_vn_regex))
					return std::make_tuple(std::stoul(match[1]) - 1, _no_index, std::stoul(match[2]) - 1);
				break;
			case FaceFormat::vtn:
				if (std::regex_match(text, match, _obj_f_vtn_regex))
					return std::make_tuple(std::stoul(match[1]) - 1, std::stoul(match[2]) - 1, std::stoul(match[3]) - 1);
				break;
			default:
				if (std::regex_match(text, match, _obj_f_v_regex))
				{
					_face_format = FaceFormat::v;
					return std::make_tuple(std::stoul(match[1]) - 1, _no_index, _no_index);
				}
				else if (std::regex_match(text, match, _obj_f_vt_regex))
				{
					_face_format = FaceFormat::vt;
					return std::make_tuple(std::stoul(match[1]) - 1, std::stoul(match[2]) - 1, _no_index);
				}
				else if (std::regex_match(text, match, _obj_f_vn_regex))
				{
					_face_format = FaceFormat::vn;
					return std::make_tuple(std::stoul(match[1]) - 1, _no_index, std::stoul(match[2]) - 1);
				}
				else if (std::regex_match(text, match, _obj_f_vtn_regex))
				{
					_face_format = FaceFormat::vtn;
					return std::make_tuple(std::stoul(match[1]) - 1, std::stoul(match[2]) - 1, std::stoul(match[3]) - 1);
				}
			}
			return {};
		}

		bool process_index(const std::tuple<size_t, size_t, size_t>& index, Yttrium::MeshData& data)
		{
			const auto index_v = std::get<0>(index);
			const auto index_t = std::get<1>(index);
			const auto index_n = std::get<2>(index);

			if (index_v >= _vertices.size() || (index_t != _no_index && index_t >= _texcoords.size()) || (index_n != _no_index && index_n >= _normals.size()))
				return false;

			const auto i = std::find(_indices.cbegin(), _indices.cend(), index);
			if (i != _indices.cend())
			{
				data._indices.emplace_back(static_cast<uint32_t>(i - _indices.cbegin()));
				return true;
			}

			Yttrium::BufferAppender<float> appender(data._vertex_data);
			appender << _vertices[index_v].x << _vertices[index_v].y << _vertices[index_v].z;
			if (index_t != _no_index)
				appender << _texcoords[index_t].x << _texcoords[index_t].y;
			if (index_n != _no_index)
				appender << _normals[index_n].x << _normals[index_n].y << _normals[index_n].z;

			data._indices.emplace_back(static_cast<uint32_t>(_indices.size()));
			_indices.emplace_back(index);
			return true;
		}

	private:
		FaceFormat _face_format = FaceFormat::unknown;
		std::vector<Yttrium::Vector3> _vertices;
		std::vector<Yttrium::Vector2> _texcoords;
		std::vector<Yttrium::Vector3> _normals;
		std::vector<std::tuple<size_t, size_t, size_t>> _indices;
	};

	// A replacement for 'boost::algorithm::trim'.
	inline void trim(std::string& s)
	{
		const auto first = s.find_first_not_of("\t\n\r ");
		const auto last = s.find_last_not_of("\t\n\r ");
		if (first != std::string::npos || last != std::string::npos)
			s = s.substr(first, last - first + 1);
		else
			s.clear();
	}
}

namespace Yttrium
{
	MeshData load_obj_mesh(const Source& source)
	{
		MeshData result;
		std::string line;
		size_t line_number = 0;
		ObjState state;
		for (Reader reader{source}; reader.read_line(line);)
		{
			++line_number;
			if (std::regex_match(line, _obj_empty_regex))
				continue;
			::trim(line);
			if (!state.process_line(line, result))
				throw DataError("OBJ processing error (", source.name(), ":", line_number, ")");
		}
		if (!state.finalize(result))
			throw DataError("Bad OBJ");
		return result;
	}
}