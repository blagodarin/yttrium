#include "tetrium.h"

namespace Tetrium
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

	bool Figure::fit(const Field& field)
	{
		int offset = 0;

		for (int i = 0; i < Blocks; ++i)
		{
			int column = blocks[i].x;
			int lower_row = blocks[i].y / PointsPerRow;
			int upper_row = (4 * blocks[i].y + 3 * PointsPerRow) / (4 * PointsPerRow);

			if (lower_row < Field::Height && field.blocks[lower_row][column] != None)
			{
				return false;
			}

			if (lower_row != upper_row)
			{
				if (upper_row < Field::Height && field.blocks[upper_row][column] != None)
				{
					return false;
				}
			}
			else
			{
				if (lower_row < Field::Height - 1 && field.blocks[lower_row + 1][column] != None)
				{
					offset = blocks[i].y - lower_row * PointsPerRow; // NOTE: Why does it rewrite the offset from the previous iteration?
				}
			}
		}

		move_down(offset);
		return true;
	}

	void Figure::move(int x, int y)
	{
		for (int i = 0; i < Blocks; ++i)
		{
			blocks[i].x += x;
			blocks[i].y += y;
		}
		_top_left.x += x;
		_top_left.y += y;
		_bottom_right.x += x;
		_bottom_right.y += y;
	}

	int Figure::move_down(const Field& field, int distance)
	{
		int max_distance = bottom();
		if (distance > max_distance)
			distance = max_distance;

		for (int i = 0; i < Blocks; ++i)
		{
			int column = blocks[i].x;
			int row = blocks[i].y / PointsPerRow - 1;
			if (row > Field::Height - 1)
				row = Field::Height - 1;
			for (; row >= 0; --row)
			{
				if (field.blocks[row][column] != None)
				{
					int altitude = blocks[i].y - (row + 1) * PointsPerRow;
					if (distance > altitude)
						distance = altitude;
					break;
				}
			}
		}

		move_down(distance);
		return distance;
	}

	bool Figure::move_horizontally(const Field& field, int cells)
	{
		// We should move the figure cell by cell to ensure
		// it can really be moved, not just teleported.

		if (cells < 0)
		{
			for (int i = cells; i < 0; ++i)
				if (!move_left(field))
					return false;
		}
		else if (cells > 0)
		{
			for (int i = 0; i < cells; ++i)
				if (!move_right(field))
					return false;
		}
		return true;
	}

	bool Figure::move_left(const Field& field)
	{
		Figure moved_figure = moved_horizontally(-1);
		if (moved_figure.check_left() && moved_figure.fit(field))
		{
			*this = moved_figure;
			return true;
		}
		return false;
	}

	bool Figure::move_right(const Field& field)
	{
		Figure moved_figure = moved_horizontally(+1);
		if (moved_figure.check_right() && moved_figure.fit(field))
		{
			*this = moved_figure;
			return true;
		}
		return false;
	}

	void Figure::set_type(Type type)
	{
		_type = type;

		switch (_type)
		{
		case I:

			// ....
			// XXXX

			blocks[0] = Point(0, 0);
			blocks[1] = Point(1, 0);
			blocks[2] = Point(2, 0);
			blocks[3] = Point(3, 0);
			_top_left = Point(0, 2 * PointsPerRow);
			_bottom_right = Point(3, -PointsPerRow);
			break;

		case J:

			// .X..
			// .XXX

			blocks[0] = Point(1, 0);
			blocks[1] = Point(2, 0);
			blocks[2] = Point(3, 0);
			blocks[3] = Point(1, PointsPerRow);
			_top_left = Point(1, PointsPerRow);
			_bottom_right = Point(3, -PointsPerRow);
			break;

		case L:

			// ..X.
			// XXX.

			blocks[0] = Point(0, 0);
			blocks[1] = Point(1, 0);
			blocks[2] = Point(2, 0);
			blocks[3] = Point(2, PointsPerRow);
			_top_left = Point(0, PointsPerRow);
			_bottom_right = Point(2, -PointsPerRow);
			break;

		case O:

			// .XX.
			// .XX.

			blocks[0] = Point(1, 0);
			blocks[1] = Point(2, 0);
			blocks[2] = Point(1, PointsPerRow);
			blocks[3] = Point(2, PointsPerRow);
			_top_left = Point(1, PointsPerRow);
			_bottom_right = Point(2, 0);
			break;

		case S:

			// ..XX
			// .XX.

			blocks[0] = Point(1, 0);
			blocks[1] = Point(2, 0);
			blocks[2] = Point(2, PointsPerRow);
			blocks[3] = Point(3, PointsPerRow);
			_top_left = Point(1, 2 * PointsPerRow);
			_bottom_right = Point(3, 0);
			break;

		case T:

			// .X..
			// XXX.

			blocks[0] = Point(0, 0);
			blocks[1] = Point(1, 0);
			blocks[2] = Point(2, 0);
			blocks[3] = Point(1, PointsPerRow);
			_top_left = Point(0, PointsPerRow);
			_bottom_right = Point(2, -PointsPerRow);
			break;

		case Z:

			// XX..
			// .XX.

			blocks[0] = Point(1, 0);
			blocks[1] = Point(2, 0);
			blocks[2] = Point(0, PointsPerRow);
			blocks[3] = Point(1, PointsPerRow);
			_top_left = Point(0, 2 * PointsPerRow);
			_bottom_right = Point(2, 0);
			break;

		default:

			// ....
			// ....

			break;
		}
	}

	bool Figure::turn_left(const Field& field)
	{
		Figure turned_figure = turned_left();
		if (turned_figure.check_left() && turned_figure.check_right()
			&& turned_figure.check_bottom() && turned_figure.fit(field))
		{
			*this = turned_figure;
			return true;
		}
		return false;
	}

	bool Figure::turn_right(const Field& field)
	{
		Figure turned_figure = turned_right();
		if (turned_figure.check_left() && turned_figure.check_right()
			&& turned_figure.check_bottom() && turned_figure.fit(field))
		{
			*this = turned_figure;
			return true;
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Figure::bottom() const
	{
		int result = blocks[0].y;
		for (int i = 1; i < Blocks; ++i)
			if (result > blocks[i].y)
				result = blocks[i].y;
		return result;
	}

	bool Figure::check_bottom() const
	{
		for (int i = 0; i < Blocks; ++i)
			if (blocks[i].y < 0)
				return false;
		return true;
	}

	bool Figure::check_left() const
	{
		for (int i = 0; i < Blocks; ++i)
			if (blocks[i].x < 0)
				return false;
		return true;
	}

	bool Figure::check_right() const
	{
		for (int i = 0; i < Blocks; ++i)
			if (blocks[i].x >= Field::Width)
				return false;
		return true;
	}

	void Figure::move_down(int distance)
	{
		for (int i = 0; i < Blocks; ++i)
			blocks[i].y -= distance;
		_top_left.y -= distance;
		_bottom_right.y -= distance;
	}

	Figure Figure::moved_horizontally(int by) const
	{
		Figure result = *this;
		for (int i = 0; i < Blocks; ++i)
			result.blocks[i].x += by;
		result._top_left.x += by;
		result._bottom_right.x += by;
		return result;
	}

	Figure Figure::turned_left() const
	{
		Figure result = *this;
		for (int i = 0; i < Blocks; ++i)
		{
			result.blocks[i] = Point(
				result._top_left.x + (result._top_left.y     - result.blocks[i].y) / PointsPerRow,
				result._top_left.y - (result._bottom_right.x - result.blocks[i].x) * PointsPerRow);
		}
		return result;
	}

	Figure Figure::turned_right() const
	{
		Figure result = *this;
		for (int i = 0; i < Blocks; ++i)
		{
			result.blocks[i] = Point(
				result._bottom_right.x - (result._top_left.y - result.blocks[i].y) / PointsPerRow,
				result._top_left.y     + (result._top_left.x - result.blocks[i].x) * PointsPerRow);
		}
		return result;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Field::clear()
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

	void Field::put_figure(const Figure& figure)
	{
		for (int i = 0; i < Figure::Blocks; ++i)
		{
			int column = figure.blocks[i].x;
			int row = figure.blocks[i].y / PointsPerRow;
			blocks[row][column] = figure.type();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	void Game::advance(unsigned milliseconds)
	{
		if (_state == Stopped || _state == Finished)
			return;

		_time_remainder += milliseconds;

		int frames = _time_remainder / MillisecondsPerFrame;
		_time_remainder %= MillisecondsPerFrame;

		for (bool horizontal = true; frames; )
		{
			switch (_state)
			{
			case Stopped:

				// NOTE: This should never happen.
				break;

			case Waiting:

				process_falldown_delay(&frames);
				break;

			case Falling:

				if (horizontal)
					_current_figure.move_horizontally(_field, process_horizontal_movement(frames));
				process_falldown(&frames);
				break;

			case Fixing:

				if (horizontal)
					_current_figure.move_horizontally(_field, process_horizontal_movement(frames));
				process_falldown(&frames);
				if (frames)
					process_fixation(&frames);
				break;

			case Finished:

				frames = 0;
				break;
			}
			horizontal = false;
		}
	}

	Figure Game::current_figure() const
	{
		return (_state == Falling || _state == Fixing) ? _current_figure : Figure();
	}

	Field Game::field() const
	{
		return (_state != Stopped) ? _field : Field();
	}

	Figure Game::next_figure() const
	{
		return (_state != Stopped) ? _next_figure : Figure();
	}

	void Game::set_left_movement(bool move)
	{
		if (_is_moving_left != move)
		{
			_is_moving_left = move;
			if (_is_moving_left)
			{
				if (_state == Falling || _state == Fixing)
					_current_figure.move_left(_field);
				_left_movement_timer = HMoveInterval - HMoveDelay;
			}
		}
	}

	void Game::set_right_movement(bool move)
	{
		if (_is_moving_right != move)
		{
			_is_moving_right = move;
			if (_is_moving_right)
			{
				if (_state == Falling || _state == Fixing)
					_current_figure.move_right(_field);
				_right_movement_timer = HMoveInterval - HMoveDelay;
			}
		}
	}

	void Game::start(int start_level)
	{
		_field.clear();
		_next_figure.set_type(random_figure_type());
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
	}

	bool Game::turn_left()
	{
		return (_state == Falling || _state == Fixing) && _current_figure.turn_left(_field);
	}

	bool Game::turn_right()
	{
		return (_state == Falling || _state == Fixing) && _current_figure.turn_right(_field);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Game::process_falldown(int* frames)
	{
		int speed = (_is_accelerating && _speed < AccelerationSpeed) ? AccelerationSpeed : _speed;
		int distance = *frames * speed;
		int actual_distance = _current_figure.move_down(_field, distance);

		if (actual_distance)
		{
			_fixation_delay = 0;
			if (_is_accelerating)
				_acceleration_bonus += actual_distance;
		}

		// The division throws away the entire contact frame, but it can't be helped.

		int remaining_frames = (distance - actual_distance) / speed;
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
			_current_figure.move((Field::Width - 4) / 2, Field::Height * PointsPerRow); // NOTE: Magic number 4 (that's NOT # of figure blocks!).
			_next_figure.set_type(random_figure_type());
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

		int lines = _field.collapse_full_rows();
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

	Figure::Type Game::random_figure_type()
	{
		_random = _random * 1664525 + 1013904223; // Knuth & Lewis roll for unsigned 32-bit numbers.
		return static_cast<Figure::Type>(_random % Figure::Count);
	}
}
