#define SIZE 16
#define LIMIT SIZE * SIZE / 2
#define random(x) (rand()%x)

class snake {
  // Public members
  public:
    char tail, hasFood, fx, fy;
    // Constructor
    snake () {
      xs[0] = 3; ys[0] = 1;
      xs[1] = 2; ys[1] = 1;
      tail = 1; hd = 4;
    }
    // Moving function
    char moveSnake (char dir) {
      // Judge whether the input direction contradicts with current direction
      if ((dir == 1 && hd == 2) || (dir == 2 && hd == 1) || (dir == 3 && hd == 4) || (dir == 4 && hd == 3)) return 2;
      // jx & jy used for judging
      char jx = xs[0], jy = ys[0];
      lx = xs[tail]; ly = ys[tail];
      if (dir == 1) jy--;
      else if (dir == 2) jy++;
      else if (dir == 3) jx--;
      else jx++;
      // Crash into wall
      if (jx >= SIZE || jx < 0 || jy >= SIZE || jy < 0) return 1;
      // Change the snake's head direction
      hd = dir;
      // If meet with food, eat it and append it to the tail
      if (jx == fx && jy == fy && hasFood == 1) {
        xs[tail + 1] = xs[tail]; ys[tail + 1] = ys[tail]; hasFood = 2;
      }
      // From tail, move its position to last snake block
      for (char i = tail; i > 0; i--) {
        xs[i] = xs[i - 1]; ys[i] = ys[i - 1];
        if (xs[i] == jx && ys[i] == jy) return 3; // Crash into self
      }
      // Go a head
      xs[0] = jx; ys[0] = jy;
      // tail++ if eat food
      if (hasFood == 2) {
        // Set to 3 for not cleaning screen
        hasFood = 3;
        // Out of memory
        if (++tail == LIMIT) return 4;
      }
      return 0;
    }
    // Print function
    void printSnake () {
      if (hasFood == 1) printBox(fx, fy, 45);
      for (char i = 0; i <= tail; i++) {
        printBox(xs[i], ys[i], (i == 0) ? 31 : 46);
      }
      // If tail appended, not wipe block
      if (hasFood == 3) hasFood = 0; else printBox(lx, ly, 47);
      Serial.println();
    }
    // Print snake block or food
    void printBox (char px, char py, char pc) {
      unsigned char tx = 8 + 160 / SIZE * px;
      unsigned char ty = 2 + 160 / SIZE * py;
      Serial.print(F("BOXF(")); Serial.print(tx); Serial.print(','); Serial.print(ty); Serial.print(',');
      Serial.print(tx + 160 / SIZE - 1); Serial.print(','); Serial.print(ty + 160 / SIZE - 1); Serial.print(',');
      Serial.print(pc,DEC); Serial.print(");");
    }
  // Private members
  private:
    char xs[LIMIT], ys[LIMIT], lx, ly, hd;
};
