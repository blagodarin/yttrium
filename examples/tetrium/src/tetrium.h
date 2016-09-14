#ifndef _examples_tetrium_tetrium_h_
#define _examples_tetrium_tetrium_h_

#include <array>

// Tetrium game logic.
namespace Tetrium
{
	enum
	{
		PointsPerRow = 40, // Logical points per row.
	};

	// Field coordinate point.
	struct Point
	{
		int x = 0;
		int y = 0;

		Point() = default;
		Point(int x, int y) : x(x), y(y) {}
	};

	class Field;

	// Data representing the falling tetromino.
	class Figure
	{
	public:

		// Figure type.
		enum Type
		{
			I, // I figure (stick).
			J, // J figure (mirrored L).
			L, // L figure (mirrored J).
			O, // O figure (square).
			S, // S figure (mirrored Z).
			T, // T figure.
			Z, // Z figure (mirrored S).

			Count, // Number of different figures.
			None,  // No figure (empty block).
		};

		Figure(Type = None);

		std::array<Point, 4> blocks() const;
		void move(int x, int y);
		int move_down(const Field&, int distance);
		bool move_horizontally(const Field&, int cells);
		bool move_left(const Field&);
		bool move_right(const Field&);
		bool turn_left(const Field&);
		bool turn_right(const Field&);
		Type type() const { return _type; }

	private:
		bool fit(const Field&);

	private:
		Type _type;
		std::array<Point, 4> _blocks; // Figure blocks coordinates (x is column, y is line).
		Point _top_left; // Top left point of the rotation rectangle.
		Point _bottom_right; // Bottom right point of the rotation rectangle.
		Point _offset; // Offset on the field.
	};

	// Data structure representing the field (blocks without the falling tetromino).
	class Field
	{
	public:
		enum
		{
			Height        = 22, // Field height.
			HeightPadding =  4, // Invisible field part height.
			Width         = 10, // Field width.
		};

		Figure::Type blocks[Height + HeightPadding][Width];

		Field();

		int collapse_full_rows();
		bool is_overflown() const;
		void put_figure(const Figure&);

	private:
		void collapse_row(int row);
		bool is_row_full(int row) const;
	};

	// Game engine.
	class Game
	{
	public:
		/// Advance the game by the specified amount of \a milliseconds.
		void advance(unsigned milliseconds);

		/// Retrieve the current figure.
		Figure current_figure() const;

		/// Retrieve the game field.
		Field field() const;

		///
		bool has_finished() const { return _state == Finished; }

		/// Get the current level.
		int level() const { return _level; }

		/// Get the number of lines cleared so far.
		int lines() const { return _lines; }

		/// Retrieve the next figure.
		Figure next_figure() const;

		/// Get the current score.
		int score() const { return _score; }

		/// Accelerate the figure.
		void set_acceleration(bool accelerate) { _is_accelerating = accelerate; }

		/// Begin or end the left move.
		/// \param move \c true if the figure should move left.
		void set_left_movement(bool move);

		/// Initialize the randomizer.
		/// \param seed Randomized seed.
		void set_random_seed(unsigned seed) { _random = seed; }

		/// Begin or end the right move.
		/// \param move \c true if the figure should move right.
		void set_right_movement(bool move);

		/// Start the game.
		void start(int start_level = 1);

		/// Turn the current figure left (counterclockwise) if possible.
		/// \return \c true on successful turn.
		bool turn_left();

		/// Turn the current figure right (clockwise) if possible.
		/// \return \c true on successful turn.
		bool turn_right();

	private:

		enum State
		{
			Stopped,  ///< The game hasn't started.
			Waiting,  ///<
			Falling,  ///<
			Fixing,   ///<
			Finished, ///< The game has finished.
		};

	private:

		void process_falldown(int* frames);
		void process_falldown_delay(int* frames);
		void process_fixation(int* frames);
		void update_score();
		int process_horizontal_movement(int frames);
		Figure::Type random_figure_type();

	private:

		State    _state = Stopped;
		int      _timeout;
		unsigned _random;
		Field    _field;
		Figure   _current_figure;
		Figure   _next_figure;
		unsigned _time_remainder;

		int      _delay;
		int      _fixation_delay;

		bool     _is_accelerating;
		int      _acceleration_bonus;

		bool     _is_moving_left;
		int      _left_movement_timer;

		bool     _is_moving_right;
		int      _right_movement_timer;

		int      _score;
		int      _lines;
		int      _level;
		int      _speed;
	};
}

#endif
