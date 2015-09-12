/// \file
/// \brief %Tetrium logic.

#ifndef YTTRIUM_EXAMPLES_TETRIUM_TETRIUM_H
#define YTTRIUM_EXAMPLES_TETRIUM_TETRIUM_H

/// %Tetrium logic.
namespace Tetrium
{

enum
{
	PointsPerRow = 40, ///< Logical points per row.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Field coordinate point.
class Point
{
public:

	int x; ///<
	int y; ///<

public:

	///
	Point(int x = 0, int y = 0)
		: x(x), y(y)
	{
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Field;

/// Data representing the falling tetromino.
class Figure
{
public:

	/// Figure type.
	enum Type
	{
		I, ///< I figure (stick).
		J, ///< J figure (mirrored L).
		L, ///< L figure (mirrored J).
		O, ///< O figure (square).
		S, ///< S figure (mirrored Z).
		T, ///< T figure.
		Z, ///< Z figure (mirrored S).

		Count, ///< Number of different figures.
		None,  ///< No figure (empty block).
	};

	enum
	{
		Blocks = 4, ///< Number of blocks in a figure.
	};

	Point blocks[Blocks]; ///< Figure blocks coordinates (x is column, y is line).

public:

	/// Create a NoFigure figure.
	Figure(): _type(None) {}

public:

	/// Try to fit the figure on the \a field, moving it slightly if necessary.
	bool fit(const Field &field);

	void move(int x, int y);

	int move_down(const Field &field, int distance);

	bool move_horizontally(const Field &field, int cells);

	bool move_left(const Field &field);

	bool move_right(const Field &field);

	void set_type(Type type);

	bool turn_left(const Field &field);

	bool turn_right(const Field &field);

	/// Get the figure type.
	Type type() const { return _type; }

private:

	int bottom() const;
	bool check_bottom() const;
	bool check_left() const;
	bool check_right() const;
	void move_down(int distance);
	Figure moved_horizontally(int by) const;
	Figure turned_left() const;
	Figure turned_right() const;

private:

	Type  _type;
	Point _top_left; // Top left point of the rotation rectangle.
	Point _bottom_right; // Bottom right point of the rotation rectangle.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Data structure representing the field (blocks without the falling tetromino).

class Field
{
public:

	enum
	{
		Height        = 22, ///< Field height.
		HeightPadding =  4, ///< Invisible field part height.
		Width         = 10, ///< Field width.
	};

public:

	Figure::Type blocks[Height + HeightPadding][Width]; ///<

public:

	/// Create an empty field.
	Field() { clear(); }

public:

	/// Clear the field.
	void clear();

	///
	int collapse_full_rows();

	///
	bool is_overflown() const;

	///
	void put_figure(const Figure &figure);

private:

	void collapse_row(int row);
	bool is_row_full(int row) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Game engine.
class Game
{
public:

	/// Constructor.
	Game(): _state(Stopped) {}

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

	/// Game state.
	enum State
	{
		Stopped,  ///< The game hasn't started.
		Waiting,  ///<
		Falling,  ///<
		Fixing,   ///<
		Finished, ///< The game has finished.
	};

private:

	void process_falldown(int *frames);
	void process_falldown_delay(int *frames);
	void process_fixation(int *frames);
	void update_score();
	int process_horizontal_movement(int frames);
	Figure::Type random_figure_type();

private:

	State    _state;
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

} // namespace Tetrium

#endif
