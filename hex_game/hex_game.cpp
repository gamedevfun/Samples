/*
 * The hex game (or game of hex) https://en.wikipedia.org/wiki/Hex_(board_game)
 * Another implementation for terminal with C++17
 * For easy compilation all code placed in one file
 * used Monte Carlo for computer player AI
 *
 */


#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <random>
#include <memory>
#include <functional>
#include <optional>
// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

using std::vector;
using std::pair;
using std::cout;
using std::cin;
using std::array;
using std::string;
using std::minstd_rand;
using std::priority_queue;
using std::unordered_map;
using std::unordered_set;
using std::optional;

enum class Color
{
  none,
  red,
  blue,
};

struct hex_cell
{
  Color color = Color::none;
};

struct position
{
  position(short column, short row)
  : column(column)
  , row(row) {}

  short column = 0;
  short row = 0;
};

class hex_board
{
  public:
    explicit hex_board(int size);
    void assign(const hex_board& other_board);
    hex_cell get_cell(int column, int row) const;
    hex_cell get_cell(int cell_index) const;
    void mark_cell(int cell_index, Color color) { hex_cells[cell_index].color = color; }
    void mark_cell(int column, int row, Color color)
    { 
      auto cell_index = to_cell_index(column, row);
      hex_cells[cell_index].color = color;
    }
    bool is_valid_position(int column, int row) const;
    int to_cell_index(int column, int row) const;
    position to_position(int cell_index) const;
    int get_size() const { return size; }
    array<short, 6> get_neighbors(int cell_index) const;
    //! draw with manipulator of output
    void draw(optional<std::function<char(int)>> manipulator);
  private:
    [[nodiscard]] hex_cell create_cell(int column, int row) const;
    void draw_line(int padding, char symbol) const;
  
    vector<hex_cell> hex_cells;
    int size;
};

hex_board::hex_board(int size)
: hex_cells(size*size)
, size(size)
{
  int index = 0;
  for(int row = 0; row < size; ++row)
  {
    for(int column = 0; column < size; ++column)
    {
      auto cell = create_cell(column, row);
      hex_cells[index] = cell;
      index++;
    }
  }
}

void hex_board::assign(const hex_board& other_board)
{
  hex_cells.assign(other_board.hex_cells.begin(), other_board.hex_cells.end());
  size = other_board.size;
}

bool hex_board::is_valid_position(int column, int row) const
{
  return row >= 0 && row < size && column >= 0 && column < size;
}

hex_cell hex_board::get_cell(int column, int row) const
{
  auto index = to_cell_index(column, row);
  return get_cell(index);
}

hex_cell hex_board::get_cell(int cell_index) const
{
  assert(cell_index >= 0 && (size_t)cell_index < hex_cells.size());
  return hex_cells[cell_index];
}

int hex_board::to_cell_index(int column, int row) const
{
  if(!is_valid_position(column, row))
    return -1;
  return row * size + column;
}

position hex_board::to_position(int cell_index) const
{
  short column = cell_index % size;
  short row = cell_index / size;
  position result {column, row};
  return result;
}

array<short, 6> hex_board::get_neighbors(int cell_index) const
{
  auto index = to_position(cell_index);
  int column = index.column, row = index.row;
  short left = to_cell_index(column - 1, row);
  short right = to_cell_index(column + 1, row);
  short top_left = to_cell_index(column, row - 1);
  short top_right = to_cell_index(column + 1, row - 1);
  short bottom_left = to_cell_index(column - 1, row + 1);
  short bottom_right = to_cell_index(column, row + 1);
  return array<short, 6> {top_left, top_right, left, right, bottom_left, bottom_right };
}

hex_cell hex_board::create_cell(int column, int row) const
{
  assert(is_valid_position(column, row));
  return hex_cell();
}

void hex_board::draw_line(int padding, char symbol) const
{
  std::ios oldState(nullptr);
  oldState.copyfmt(std::cout);
  
  cout<<std::setw(padding);
  for(int i = 0; i < size; ++i)
    cout<<std::hex<<i<<' ';
  cout<<'\n';

  cout<<std::setw(padding);
  for(int i = 0; i < size; ++i)
    cout << symbol << ' ';
  cout<<'\n';
  
  std::cout.copyfmt(oldState);
}

void hex_board::draw(optional<std::function<char(int)>> manipulator = std::nullopt)
{
  const int padding = 5;
  draw_line(padding+4, 'R');
  
  // save params of output, and restore them later
  std::ios oldState(nullptr);
  oldState.copyfmt(std::cout);
  
  for(int i = 0, index = 0; i < size; ++i)
  {
    cout<<std::setw(padding+i);
    cout<<std::hex<<i;
    cout<<" B ";
    for(int j = 0; j < size; ++j, index++)
    {
      auto value = hex_cells[index].color;
      
      // if manipulator return '\0' (zero), that's shows no need to modify output
      auto symbol = manipulator.has_value() ? manipulator.value()(index) : '\0';
      if(symbol == '\0')
      {
        if(value == Color::red)
          cout<<'R';
        else if(value == Color::blue)
          cout<<'B';
        else
          cout<<'.';
      }
      else
      {
        cout<<symbol;
      }
      
      if(j != size-1)
        cout<<"-";
    }
    
    cout<<" B ";
    cout<<std::hex<<i;
    
    if(i != size-1)
    {
      cout<<'\n';
      cout<<std::setw(padding+5+i);
      for(int j = 0; j < size+size-1; ++j)
      {
        if(j % 2 == 0)
          cout<<"\\";
        else
          cout<<"/";
      }
    }
    
    cout<<'\n';
  }
  // restore output params
  std::cout.copyfmt(oldState);
  
  draw_line(padding+4+size-1, 'R');
  
  cout<<"\n\n"<<std::endl;
}

class path_finder
{
public:
    static bool search_path(const hex_board& board, const int cell_from, const int cell_to, unordered_map<int, int>* out_predecessor = nullptr)
    {
      // priority queue by path cost
      auto cmp = [](cell_cost left, cell_cost right) { return left.cost > right.cost; };
      priority_queue<cell_cost, std::vector<cell_cost>, decltype(cmp)> open_set(cmp);
      
      // for reconstruct path, "predecessor[B] = A" records A as the predecessor of B, meaning that A discovered B.
      auto predecessor = out_predecessor;
      // initial value
      open_set.push(cell_cost{cell_from, 0});
      
      unordered_set<int> closed_set;
      
      while(!open_set.empty())
      {
        auto next = open_set.top();
        open_set.pop();
        
        auto cell = next.cell;
        int cost = next.cost;
        
        closed_set.insert(cell);
        // stop search if found destination cell
        if(cell == cell_to)
          return true;
        
        auto neighbors = board.get_neighbors(cell);
        for(auto neighbor : neighbors)
        {
          if(neighbor != -1)
          {
            auto new_cell = neighbor;
            if(board.get_cell(new_cell).color != board.get_cell(cell_from).color)
              continue;
            // if in closed set go to next neighbor 
            if(closed_set.count(new_cell) > 0)
              continue;
            // if have optionally param predecessor, write to it
            if(predecessor != nullptr && (*predecessor).count(new_cell) == 0)
              (*predecessor)[new_cell] = cell;
            // all cost growing by 1
            open_set.push(cell_cost{new_cell, cost+1});
          }
        }
      }
      
      return false;
    }
    
private:
  struct cell_cost
  {
    int cell;
    int cost;
  };
};

// Abstract player base class for hex game player
class base_player
{
  public:
    explicit base_player(Color color)
    : color(color) {}
    
    Color get_color() const { return color; }
    void capture_cell(int cell_index) { captured_cells.push_back(cell_index); }
    const vector<int>& get_captured_cells() const { return captured_cells; }

    virtual position make_move(const hex_board& board) = 0;
    
  private:
    Color color;
    vector<int> captured_cells;
};

// Game rules for determine win and get path that player build
class HexGameRules
{
public:

  Color check_winner(const hex_board& board, vector<int>* out_win_path = nullptr)
  {
    vector<int> one_side_cells;
    vector<int> another_side_cells;
    for(int i = 0; i < board.get_size(); ++i)
    {
      if(board.get_cell(0, i).color == Color::blue)
        one_side_cells.push_back(board.to_cell_index(0, i));
      if(board.get_cell(board.get_size()-1, i).color == Color::blue)
        another_side_cells.push_back(board.to_cell_index(board.get_size() - 1, i));
    }
    
    if(is_win(board, one_side_cells, another_side_cells, out_win_path))
      return Color::blue;
    
    one_side_cells.clear();
    another_side_cells.clear();
    for(int i = 0; i < board.get_size(); ++i)
    {
      if(board.get_cell(i, 0).color == Color::red)
        one_side_cells.push_back(board.to_cell_index(0, i));
      if(board.get_cell(i, board.get_size()-1).color == Color::red)
        another_side_cells.push_back(board.to_cell_index(i, board.get_size() - 1));
    }
    
    if(is_win(board, one_side_cells, another_side_cells, out_win_path))
      return Color::red;
    
    return Color::none;
  }

  bool is_winner(const hex_board& board, const base_player& player, vector<int>* out_win_path = nullptr)
  {
    // first get all hex cells on the player win side
    auto captured_cells = player.get_captured_cells();
    vector<int> one_side_cells;
    vector<int> another_side_cells;
    for(auto n_id : captured_cells)
    {
      auto indexes = board.to_position(n_id);
      // get all blue cells on destination side from start to end
      if(player.get_color() == Color::blue)
      {
        if(indexes.column == 0)
          one_side_cells.push_back(n_id);
        else if(indexes.column == board.get_size() - 1)
          another_side_cells.push_back(n_id);
      }
      // get all red cells on destination side from start to end
      else
      {
        if(indexes.row == 0)
          one_side_cells.push_back(n_id);
        else if(indexes.row == board.get_size() - 1)
          another_side_cells.push_back(n_id);
      }

    }
    
    return is_win(board, one_side_cells, another_side_cells, out_win_path);
  }
  
protected:

  static void reconstruct_path(int destination_cell, const unordered_map<int, int>& predecessor, vector<int>& out_path)
  {
    auto target = destination_cell;
    out_path.push_back(target);

    while (predecessor.count(target))
    {
      auto next_pred = predecessor.at(target);
      out_path.push_back(next_pred);
      target = next_pred;
    }
  }

  static bool is_win(const hex_board& board, const vector<int>& one_side_cells, const vector<int>& another_side_cells, vector<int>* out_win_path = nullptr)
  {
    auto predecessor = out_win_path != nullptr ? optional<unordered_map<int, int>> {} : std::nullopt;
    auto out_predecessor = predecessor.has_value() ? &predecessor.value() : nullptr;

    // if has two cells on two sides
    if(!one_side_cells.empty() && !another_side_cells.empty())
    {
      for(auto side_cell : one_side_cells)
      {
        for(auto another_side_cell : another_side_cells)
        {
          auto result = path_finder::search_path(board, side_cell, another_side_cell, out_predecessor);
          if(result)
          {
            if(predecessor.has_value())
              reconstruct_path(another_side_cell, predecessor.value(), (*out_win_path));
            return result;
          }
        }
      }
    }
    
    return false;
  }
};

class player_human : public base_player
{
  using base_player::base_player;
  
public:

  position make_move(const hex_board&) override
  {
    cout<<"Enter position: column row \n";
    string r_str, c_str;
    cin >> r_str;
    cin >> c_str;
    short row = std::stoi(r_str, nullptr, 16);
    short column = std::stoi(c_str, nullptr, 16);
    return position {row, column};
  }
};

class player_cpu : public base_player
{
public:
  explicit player_cpu(Color color)
  : base_player(color)
  , random(time(nullptr))
  { }

  position make_move(const hex_board& board) override
  {
    vector<position> valid_cells;
    unordered_map<int, int> win_table;

    const auto size = board.get_size();
    for(int row = 0; row < size; ++row)
    {
      for(int column = 0; column < size; ++column)
      {
        auto index = board.to_cell_index(column, row);
        if(board.get_cell(index).color != Color::none)
          continue;

        valid_cells.emplace_back(column, row);
        // win count set to 0
        win_table[index] = 0;
      }
    }
    
    HexGameRules hex_rules;
    hex_board board_copy(board);
    auto valid_moves_count = valid_cells.size();
    vector<position> valid_cells_copy;
    valid_cells_copy.reserve(valid_moves_count);
    for(unsigned int try_index = 0; try_index < monte_carlo_iteration_count; ++try_index)
    {
      board_copy.assign(board);
      valid_cells_copy.assign(valid_cells.begin(), valid_cells.end());
      auto next_player_color = this->get_color();
      for(unsigned int j = 0; j < valid_moves_count; ++j)
      {
        auto chosen_move_index = random() % valid_cells_copy.size();
        auto move_pos = valid_cells_copy[chosen_move_index];
        board_copy.mark_cell(move_pos.column, move_pos.row, next_player_color);
        next_player_color = next_player_color == Color::blue ? Color::red : Color::blue;
        
        // fast erase, it changes order, but for this it's ok
        if(valid_cells_copy.size() > 1)
          std::swap(valid_cells_copy.back(), valid_cells_copy[chosen_move_index]);
        valid_cells_copy.pop_back();
      }
      
      //! if player win game add 1 or add -1 if not
      int value = hex_rules.check_winner(board_copy) == this->get_color() ? 1 : -1;
      
      for(auto pos : valid_cells)
      {
        auto cell_index = board.to_cell_index(pos.column, pos.row);
        if(board_copy.get_cell(cell_index).color == this->get_color())
          win_table[cell_index]+=value;
      }
    }
    
    auto result = std::max_element(win_table.begin(), win_table.end(), [] (const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; } );
    return board.to_position(result->first);
  }
  
private:
  minstd_rand random;
  const unsigned int monte_carlo_iteration_count = 2600;
};

class hex_game : public HexGameRules
{
  public:
    explicit hex_game(int board_size)
    : board(board_size)
    , player_blue(Color::blue)
    , player_red(Color::red)
    {
    }
    
    void run_loop()
    {
      while(true)
      {
        board.draw();

        make_valid_move(player_blue);
        if(check_is_winner(player_blue, "Blue"))
          return;
        
        board.draw();

        make_valid_move(player_red);
        if(check_is_winner(player_red, "Red"))
          return;
      }
    }
    
    void make_valid_move(base_player& player)
    {
      while(true)
      {
        auto move = player.make_move(board);
        if(!board.is_valid_position(move.column, move.row))
        {
          cout<<"Error! row and column not valid for size of board\n";
          cout<<"Please try again with valid values\n ";
          continue;
        }
        else if(board.get_cell(move.column, move.row).color != Color::none)
        {
          auto color = board.get_cell(move.column, move.row).color;
          cout<<"Error! This row and column already taken by "<<(color == Color::blue ? "blue" : "red")<<" \n";
          cout<<"Please enter valid values\n ";
          continue;
        }
        
        auto cell_index = board.to_cell_index(move.column, move.row);
        board.mark_cell(cell_index, player.get_color());
        player.capture_cell(cell_index);
        
        break;
      }
    }
    
    bool check_is_winner(const base_player& player, const string& name)
    {
      vector<int> win_path;
      if(is_winner(board, player, &win_path))
      {
        auto modify_win_path = [&](int cell_index)
          {
            auto find_result = std::find(win_path.begin(), win_path.end(), cell_index);
            if(find_result != win_path.end())
            {
              auto color = board.get_cell(cell_index).color;
              if(color == Color::blue)
                return 'b';
              else if(color == Color::red)
                return 'r';
            }
            // do not modify output
            return static_cast<char>(0);
          };
        board.draw(modify_win_path);
        
        cout<<"\n***********************\n";
        cout<<"\n Player "<<name<<" is winner!\n\n";
        cout<<"\n***********************\n\n";
        
        return true;
      }
      return false;
    }
      
private:
  hex_board board;
  player_human player_blue;
  player_cpu player_red;
};

int main(int, char **)
{  
  hex_game game(11);
  game.run_loop();
  
  return 0;
}
