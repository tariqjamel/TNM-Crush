#include <SFML/Graphics.hpp>
#include <time.h>
#include <SFML/Graphics/Text.hpp>

using namespace sf;

int ts = 54; // tile size
Vector2i offset(48, 24);

class Piece;  // Forward declaration

Piece grid[9][9];  // Forward declaration of grid

class GameObject {
public:
    int x, y, col, row, kind, match, alpha;
    GameObject() : match(0), alpha(255) {}

    virtual void draw(Sprite& gems, const Vector2i& offset) = 0;  // Pure virtual function for drawing
};

class Piece : public GameObject {
public:
    void swap(Piece& other);

    void draw(Sprite& gems, const Vector2i& offset) override {
        gems.setTextureRect(IntRect(kind * 49, 0, 49, 49));
        gems.setColor(Color(255, 255, 255, alpha));
        gems.setPosition(x, y);
        gems.move(offset.x - ts, offset.y - ts);
    }
};  

void Piece::swap(Piece& other) {
    std::swap(col, other.col);
    std::swap(row, other.row);
    std::swap(kind, other.kind);

    // Update the grid with the swapped pieces
    grid[row][col] = *this;
    grid[other.row][other.col] = other;
}

int main() {
    srand(time(0));

    RenderWindow app(VideoMode(740, 480), "TNM crush");
    app.setFramerateLimit(60);

    Texture t1, t2;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/gems.png");

    Sprite background(t1), gems(t2);

    Font font;
    font.loadFromFile("path/to/your/font.ttf"); // Replace with the path to your font file

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10); // Adjust the position as needed
    int score = 0;

    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= 8; j++) {
            grid[i][j].kind = rand() % 3;
            grid[i][j].col = j;
            grid[i][j].row = i;
            grid[i][j].x = j * ts;
            grid[i][j].y = i * ts;
        }

    int x0, y0, x, y;
    int click = 0;
    Vector2i pos;
    bool isSwap = false, isMoving = false;

    while (app.isOpen()) {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();

            if (e.type == Event::MouseButtonPressed)
                if (e.key.code == Mouse::Left) {
                    if (!isSwap && !isMoving)
                        click++;
                    pos = Mouse::getPosition(app) - offset;
                }
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
                app.close();
        }

        // Calculate score
        score = 0;
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
                score += grid[i][j].match;
    
   // mouse click
  if (click == 1) {
    x0 = pos.x / ts + 1;
    y0 = pos.y / ts + 1;
}
if (click == 2) {
    x = pos.x / ts + 1;
    y = pos.y / ts + 1;
    if (abs(x - x0) + abs(y - y0) == 1) {
        grid[y0][x0].swap(grid[y][x]);
        isSwap = 1;
        click = 0;
    } else
        click = 1;
}

   //Match finding
   for(int i=1;i<=8;i++)
   for(int j=1;j<=8;j++)
   {
    if (grid[i][j].kind==grid[i+1][j].kind)
    if (grid[i][j].kind==grid[i-1][j].kind)
     for(int n=-1;n<=1;n++) grid[i+n][j].match++;

    if (grid[i][j].kind==grid[i][j+1].kind)
    if (grid[i][j].kind==grid[i][j-1].kind)
     for(int n=-1;n<=1;n++) grid[i][j+n].match++;
   }

   //Moving animation
  isMoving = false;
for (int i = 1; i <= 8; i++)
    for (int j = 1; j <= 8; j++) {
        Piece &p = grid[i][j];
        int dx = 0, dy = 0; // Declare dx and dy outside the loop

        for (int n = 0; n < 4; n++) { // 4 - speed
            dx = p.x - p.col * ts;
            dy = p.y - p.row * ts;
            if (dx) p.x -= dx / abs(dx);
            if (dy) p.y -= dy / abs(dy);
        }

        if (dx || dy) isMoving = true;
    }

   //Deleting amimation
   if (!isMoving)
    for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
    if (grid[i][j].match) if (grid[i][j].alpha>10) {grid[i][j].alpha-=10; isMoving=true;}

   //Get score
   int score=0;
   for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
      score+=grid[i][j].match;

   //Second swap if no match
    if (isSwap && !isMoving) {
            if (!score) {
                // Swap the pieces directly
                grid[y0][x0].swap(grid[y][x]);
            }
            isSwap = false;
        }

        // Update grid
        if (!isMoving) {
            for (int i = 8; i > 0; i--)
                for (int j = 1; j <= 8; j++)
                    if (grid[i][j].match)
                        for (int n = i; n > 0; n--)
                            if (!grid[n][j].match) {
                                // Swap the pieces directly
                                grid[n][j].swap(grid[i][j]);
                                break;
                            }
        }

        for (int j = 1; j <= 8; j++)
            for (int i = 8, n = 0; i > 0; i--)
                if (grid[i][j].match) {
                    grid[i][j].kind = rand() % 7;
                    grid[i][j].y = -ts * n++;
                    grid[i][j].match = 0;
                    grid[i][j].alpha = 255;
                }
    }

    // Draw
    app.clear();

    app.draw(background);

    for (int i = 1; i <= 8; i++)
        for (int j = 1; j <= 8; j++) {
            Piece& p = grid[i][j];
            p.draw(gems, offset);
            app.draw(gems);
        }

    // Update and draw the score
    scoreText.setString("Score: " + std::to_string(score));
    app.draw(scoreText);

    app.display();
    return 0;

}