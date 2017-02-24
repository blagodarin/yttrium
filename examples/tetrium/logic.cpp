#include "logic.h"

#include <cassert>

namespace
{
	// Game timing parameters.
	enum
	{
		MillisecondsPerFrame = 25, // 25 ms frames (40 frames per second).

		// Delays.

		StartupDelay         = 20, // 0.5 s startup delay.
		FixDelay             = 16, // 0.4 s delay before the figure fixation.
		DelayFrames          =  4, // 0.1 s delay between the fixation and the next figure.

		// Speed values.

		BaseSpeed            =  1, // 1 point per frame (1 cell per second).
		AccelerationSpeed    = 80, // 80 points (2 cells) per frame (80 cells per second).

		// Horizontal movement timing.

		HMoveDelay           =  6, // 150 ms initial delay.
		HMoveInterval        =  2, // 50 ms successive delays.
	};

	enum
	{
		MaxFigureWidth = 4,
	};
}

namespace Tetrium
{
	Figure::Figure(Type type)
		: _type(type)
	{
		switch (_type)
		{
		case I:
			// ....
			// XXXX
			_blocks[0] = { 0, 0 };
			_blocks[1] = { 1, 0 };
			_blocks[2] = { 2, 0 };
			_blocks[3] = { 3, 0 };
			_top_left = { 0, 2 };
			_bottom_right = { 3, -1 };
			break;

		case J:
			// .X..
			// .XXX
			_blocks[0] = { 1, 0 };
			_blocks[1] = { 2, 0 };
			_blocks[2] = { 3, 0 };
			_blocks[3] = { 1, 1 };
			_top_left = { 1, 1 };
			_bottom_right = { 3, -1 };
			break;

		case L:
			// ..X.
			// XXX.
			_blocks[0] = { 0, 0 };
			_blocks[1] = { 1, 0 };
			_blocks[2] = { 2, 0 };
			_blocks[3] = { 2, 1 };
			_top_left = { 0, 1 };
			_bottom_right = { 2, -1 };
			break;

		case O:
			// .XX.
			// .XX.
			_blocks[0] = { 1, 0 };
			_blocks[1] = { 2, 0 };
			_blocks[2] = { 1, 1 };
			_blocks[3] = { 2, 1 };
			_top_left = { 1, 1 };
			_bottom_right = { 2, 0 };
			break;

		case S:
			// ..XX
			// .XX.
			_blocks[0] = { 1, 0 };
			_blocks[1] = { 2, 0 };
			_blocks[2] = { 2, 1 };
			_blocks[3] = { 3, 1 };
			_top_left = { 1, 2 };
			_bottom_right = { 3, 0 };
			break;

		case T:
			// .X..
			// XXX.
			_blocks[0] = { 0, 0 };
			_blocks[1] = { 1, 0 };
			_blocks[2] = { 2, 0 };
			_blocks[3] = { 1, 1 };
			_top_left = { 0, 1 };
			_bottom_right = { 2, -1 };
			break;

		case Z:
			// XX..
			// .XX.
			_blocks[0] = { 1, 0 };
			_blocks[1] = { 2, 0 };
			_blocks[2] = { 0, 1 };
			_blocks[3] = { 1, 1 };
			_top_left = { 0, 2 };
			_bottom_right = { 2, 0 };
			break;

		default:
			// ....
			// ....
			break;
		}
	}

	std::array<Point, 4> Figure::blocks() const
	{
		std::array<Point, 4> result;
		for (const auto& block : _blocks)
			result[&block - _blocks.data()] = { _offset.x + block.x, _offset.y + block.y * PointsPerRow };
		return result;
	}

	void Figure::turn_left()
	{
		for (auto& block : _blocks)
			block = { _top_left.x + (_top_left.y - block.y), _top_left.y - (_bottom_right.x - block.x) };
	}

	void Figure::turn_right()
	{
		for (auto& block : _blocks)
			block = { _bottom_right.x - (_top_left.y - block.y), _top_left.y + (_top_left.x - block.x) };
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Field::Field()
	{
		for (int y = 0; y < Height + HeightPadding; ++y)
			for (int x = 0; x < Width; ++x)
				blocks[y][x] = Figure::None;
	}

	int Field::collapse_full_rows()
	{
		int result = 0;
		for (int row = Field::Height - 1; row >= 0; --row)
		{
			if (is_row_full(row))
			{
				collapse_row(row);
				++result;
			}
		}
		return result;
	}

	bool Field::is_overflown() const
	{
		for (int row = Height; row < Height + HeightPadding; ++row)
			for (int column = 0; column < Width; ++column)
				if (blocks[row][column] != Figure::None)
					return true;
		return false;
	}

	bool Field::fit(Figure& figure) const
	{
		const auto row_offset = figure.row_offset();
		bool shift = false;
		for (const auto& block : figure.blocks())
		{
			if (block.x < 0 || block.x >= Width || block.y < 0)
				return false;
			const auto row = block.y / PointsPerRow;
			if (row < Height && blocks[row][block.x] != Figure::None)
				return false;
			if (row + 1 < Height && blocks[row + 1][block.x] != Figure::None)
			{
				if (row_offset >= PointsPerRow / 4)
					return false;
				shift = true;
			}
		}
		if (shift)
			figure.move_down(row_offset);
		return true;
	}

	void Field::put_figure(const Figure& figure)
	{
		for (const auto& block : figure.blocks())
			blocks[block.y / PointsPerRow][block.x] = figure.type();
	}

	void Field::collapse_row(int row)
	{
		for (int y = row; y < Field::Height; ++y)
			for (int x = 0; x < Field::Width; ++x)
				blocks[y][x] = blocks[y + 1][x];
	}

	bool Field::is_row_full(int row) const
	{
		int cells_filled = 0;
		for (int column = 0; column < Field::Width; ++column)
			cells_filled += (blocks[row][column] != Figure::None);
		return cells_filled == Width;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Game::Game()
	{
	}

	bool Game::advance(unsigned milliseconds)
	{
		if (!is_active())
			return false;

		_time_remainder += milliseconds;

		int frames = _time_remainder / MillisecondsPerFrame;
		_time_remainder %= MillisecondsPerFrame;

		for (bool horizontal = true; frames; )
		{
			switch (_state)
			{
			case Stopped:
				assert(false);
				break;

			case Waiting:
				process_falldown_delay(&frames);
				break;

			case Falling:
			case Fixing:
				if (horizontal)
				{
					for (auto offset = process_horizontal_movement(frames); offset;)
					{
						auto figure = _current_figure;
						if (offset > 0)
						{
							figure.move_right();
							--offset;
						}
						else
						{
							figure.move_left();
							++offset;
						}
						if (!_field.fit(figure))
							break;
						_current_figure = figure;
					}
				}
				process_falldown(&frames);
				if (_state == Fixing && frames)
					process_fixation(&frames);
				break;

			case Finished:
				frames = 0;
				break;
			}
			horizontal = false;
		}
		return true;
	}

	Figure Game::current_figure() const
	{
		return (_state == Falling || _state == Fixing) && !_paused ? _current_figure : Figure();
	}

	Field Game::field() const
	{
		return _state != Stopped && !_paused ? _field : Field();
	}

	bool Game::is_active() const
	{
		return _state != Stopped && _state != Finished && !_paused;
	}

	Figure Game::next_figure() const
	{
		return _state != Stopped && !_paused ? _next_figure : Figure();
	}

	void Game::pause()
	{
		if (_state != Stopped && _state != Finished)
			_paused = true;
	}

	void Game::resume()
	{
		if (_state != Stopped && _state != Finished)
			_paused = false;
	}

	void Game::set_acceleration(bool accelerate)
	{
		if (is_active())
			_is_accelerating = accelerate;
	}

	void Game::set_left_movement(bool move)
	{
		if (!is_active() || _is_moving_left == move)
			return;
		_is_moving_left = move;
		if (_is_moving_left)
		{
			if (_state != Waiting)
			{
				auto figure = _current_figure;
				figure.move_left();
				if (_field.fit(figure))
					_current_figure = figure;
			}
			_left_movement_timer = HMoveInterval - HMoveDelay;
		}
	}

	void Game::set_right_movement(bool move)
	{
		if (!is_active() || _is_moving_right == move)
			return;
		_is_moving_right = move;
		if (_is_moving_right)
		{
			if (_state != Waiting)
			{
				auto figure = _current_figure;
				figure.move_right();
				if (_field.fit(figure))
					_current_figure = figure;
			}
			_right_movement_timer = HMoveInterval - HMoveDelay;
		}
	}

	void Game::start(int start_level)
	{
		_field = {};
		_next_figure = Figure(static_cast<Figure::Type>(_random_distribution(_random_engine)));
		_time_remainder = 0;

		_is_accelerating = false;
		_acceleration_bonus = 0;

		_is_moving_left = false;
		_left_movement_timer = 0;

		_is_moving_right = false;
		_right_movement_timer = 0;

		_score = 0;
		_lines = 0;
		_level = start_level;
		_speed = BaseSpeed * _level;

		_state = Waiting;
		_timeout = StartupDelay;

		_paused = false;
	}

	bool Game::turn_left()
	{
		if (!is_active() || _state == Waiting)
			return false;
		auto figure = _current_figure;
		figure.turn_left();
		if (!_field.fit(figure))
			return false;
		_current_figure = figure;
		return true;
	}

	bool Game::turn_right()
	{
		if (!is_active() || _state == Waiting)
			return false;
		auto figure = _current_figure;
		figure.turn_right();
		if (!_field.fit(figure))
			return false;
		_current_figure = figure;
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Game::process_falldown(int* frames)
	{
		const auto speed = (_is_accelerating && _speed < AccelerationSpeed) ? AccelerationSpeed : _speed;
		const auto expected_distance = *frames * speed;

		auto actual_distance = expected_distance;
		for (const auto& block : _current_figure.blocks())
		{
			actual_distance = std::min(actual_distance, block.y);
			for (auto row = std::min(block.y / PointsPerRow, int{ Field::Height }); row > 0; --row)
			{
				if (_field.blocks[row - 1][block.x] != Figure::None)
				{
					actual_distance = std::min(actual_distance, block.y - row * PointsPerRow);
					break;
				}
			}
		}
		_current_figure.move_down(actual_distance);

		if (actual_distance)
		{
			_fixation_delay = 0;
			if (_is_accelerating)
				_acceleration_bonus += actual_distance;
		}

		// The division throws away the entire contact frame, but it can't be helped.

		int remaining_frames = (expected_distance - actual_distance) / speed;
		*frames = remaining_frames;

		if (remaining_frames && _state != Fixing)
		{
			_state = Fixing;
			_timeout = FixDelay;
		}
	}

	void Game::process_falldown_delay(int* frames)
	{
		int remaining_frames = *frames;
		if (_timeout > remaining_frames)
		{
			_timeout -= remaining_frames;
			remaining_frames = 0;
		}
		else
		{
			remaining_frames -= _timeout;
			_timeout = 0;
		}

		int delay_frames = *frames - remaining_frames;
		if (delay_frames)
			process_horizontal_movement(delay_frames); // Update the timers without moving the figure.
		*frames = remaining_frames;

		if (!_timeout)
		{
			_state = Falling;
			_current_figure = _next_figure;
			_current_figure.put_at({ (Field::Width - MaxFigureWidth) / 2, Field::Height * PointsPerRow });
			_next_figure = Figure(static_cast<Figure::Type>(_random_distribution(_random_engine)));
		}
	}

	void Game::process_fixation(int* frames)
	{
		int remaining_frames = *frames;
		if (_timeout > remaining_frames)
		{
			_timeout -= remaining_frames;
			remaining_frames = 0;
		}
		else
		{
			remaining_frames -= _timeout;
			_timeout = 0;
		}
		*frames = remaining_frames;

		if (!_timeout)
		{
			_field.put_figure(_current_figure);
			update_score();
			if (_field.is_overflown())
			{
				_state = Finished;
				_paused = false;
			}
			else
			{
				_state = Waiting;
				_timeout = DelayFrames;
				_speed = BaseSpeed * _level;
			}
		}
	}

	void Game::update_score()
	{
		// The scoring system used here is based on Nintendo Tetris.

		_score += _acceleration_bonus / PointsPerRow; // One bonus point per accelerated row.
		_acceleration_bonus = 0;

		static int score_table[] = { 0, 40, 100, 300, 1200 };

		const auto lines = _field.collapse_full_rows();
		_score += score_table[lines] * _level;
		_lines += lines;
		_level += _lines / Field::Width - (_lines - lines) / Field::Width;
	}

	int Game::process_horizontal_movement(int frames)
	{
		int offset = 0;

		if (_is_moving_left)
		{
			_left_movement_timer += frames;
			while (_left_movement_timer >= HMoveInterval)
			{
				_left_movement_timer -= HMoveInterval;
				--offset;
			}
		}

		if (_is_moving_right)
		{
			_right_movement_timer += frames;
			while (_right_movement_timer >= HMoveInterval)
			{
				_right_movement_timer -= HMoveInterval;
				++offset;
			}
		}

		return offset;
	}
}
