#
pragma config(Motor, motorA, RightMotor, tmotorEV3_Large, PIDControl, encoder)# pragma config(Motor, motorB, LeftMotor, tmotorEV3_Large, PIDControl, encoder)# pragma config(Motor, motorD, Dispenser, tmotorEV3_Medium, PIDControl, encoder)

const int ScreenHeight = 127;
const int ScreenWidth = 177;

typedef struct {
  int NorthWall;
  int EastWall;
  int SouthWall;
  int WestWall;
}
Cell;

//Array based stack for the shortest path
const int length = 50;
int arrStack[length];
int stackTrack = 0;

Cell Grid[4][6];

//Start Facing North
int RobotDirection = 0; //0=North, 1=East, 2=South, 3=West

int StartPosRow = 3;
int StartPosCol = 5;

int CurrentPosRow = StartPosRow;
int CurrentPosCol = StartPosCol;

//End positions rotates through each bins coordinates
int TargetRows[5] = {
  3,
  1,
  2,
  3,
  3
};
int TargetCols[5] = {
  2,
  5,
  0,
  3,
  2
};

int TargetPosRow; //Bin Row
int TargetPosCol; //Bin Col
int targetCounter = 0; //Iteration counter to next bin

int counter = 0; //First bin counter
bool flag = true;

/**************FUNCTION DECLARATIONS**************/

void stackPush(int * Arr, int n);
void stackPop(int * Arr);
void moveRobot();
void turnRobot(int dir);
void moveForward(int distance);
void dispenseBall();
void GridInit();
void WallGen();
void GridDraw();
void DrawBot();
void DisplayStartandEnd();
void GoFwd();
void TurnLeft();
void TurnRight();

int Solver(); //Return 0 if wall no wall found, -1 if wall found based in Checkwall();
int CheckWall(); //Checks if wall in front, returns 1 if yes, 0 if no

/**************END OF FUNCTION DECLARATIONS**************/

task main() {
  GridInit();
  WallGen();
  for (int i = 1; i < length; i++) { //Initialize array to -99's
    arrStack[i] = -99;
  }
  arrStack[0] = 0;

  bool track = true;
  while (track) {
    TargetPosRow = TargetRows[targetCounter];
    TargetPosCol = TargetCols[targetCounter];
    while ((CurrentPosRow != TargetPosRow) || (CurrentPosCol != TargetPosCol)) {
      int temp = Solver();
      GridDraw();
      DisplayStartandEnd();
      DrawBot();
      sleep(5);
      eraseDisplay();
      if (flag == true) {
        counter++;
      }
    }
    flag = false;
    stackTrack++;
    targetCounter++;
    if ((targetCounter % 5) == 0) {
      track = false;
    }
  }

  CurrentPosRow = 3;
  CurrentPosCol = 5;
  RobotDirection = 0;
  moveRobot();
}

/************************************FUNCTIONS************************************/
void GridInit() { //Initilizes the maze as a grid
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      Grid[i][j].NorthWall = 0;
      Grid[i][j].EastWall = 0;
      Grid[i][j].WestWall = 0;
      Grid[i][j].SouthWall = 0;
    }
  }
}

void WallGen() {
  int i = 0;
  int j = 0;
  for (i = 0; i < 4; i++) { //Sets walls for left and right, i.e Grid[0][0].EastWall
    Grid[i][0].WestWall = 1;
    Grid[i][5].EastWall = 1;
  }
  for (j = 0; j < 6; j++) { //Sets walls for top and bottom
    Grid[0][j].SouthWall = 1;
    Grid[3][j].NorthWall = 1;
  }
  //Sets walls for each row
  Grid[0][0].NorthWall = 1; //First row (bottom)
  Grid[0][1].EastWall = 1;
  Grid[0][2].WestWall = 1;
  Grid[0][2].EastWall = 1;
  Grid[0][3].WestWall = 1;
  Grid[0][3].EastWall = 1;
  Grid[0][4].WestWall = 1;
  Grid[0][5].NorthWall = 1;

  Grid[1][0].SouthWall = 1; //Second row
  Grid[1][1].NorthWall = 1;
  Grid[1][2].EastWall = 1;
  Grid[1][3].WestWall = 1;
  Grid[1][3].EastWall = 1;
  Grid[1][4].WestWall = 1;
  Grid[1][4].EastWall = 1;
  Grid[1][5].WestWall = 1;
  Grid[1][5].SouthWall = 1;

  Grid[2][0].NorthWall = 1;
  Grid[2][0].EastWall = 1; //Third row
  Grid[2][1].WestWall = 1;
  Grid[2][1].SouthWall = 1;
  Grid[2][2].NorthWall = 1;
  Grid[2][5].NorthWall = 1;

  Grid[3][0].SouthWall = 1; //Last row (top)
  Grid[3][2].SouthWall = 1;
  Grid[3][2].EastWall = 1;
  Grid[3][3].WestWall = 1;
  Grid[3][3].EastWall = 1;
  Grid[3][4].WestWall = 1;
  Grid[3][5].SouthWall = 1;
} //End of WallGen();

void GridDraw() { //Draws grid
  int XStart = 0;
  int YStart = 0;
  int XEnd = 0;
  int YEnd = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      if (Grid[i][j].NorthWall == 1) {
        XStart = j * ScreenWidth / 6;
        YStart = (i + 1) * ScreenHeight / 4;
        XEnd = (j + 1) * ScreenWidth / 6;
        YEnd = (i + 1) * ScreenHeight / 4;
        drawLine(XStart, YStart, XEnd, YEnd);
      }
      if (Grid[i][j].EastWall == 1) {
        XStart = (j + 1) * ScreenWidth / 6;
        YStart = (i) * ScreenHeight / 4;
        XEnd = (j + 1) * ScreenWidth / 6;
        YEnd = (i + 1) * ScreenHeight / 4;
        drawLine(XStart, YStart, XEnd, YEnd);
      }
      if (Grid[i][j].WestWall == 1) {
        XStart = j * ScreenWidth / 6;
        YStart = (i) * ScreenHeight / 4;
        XEnd = (j) * ScreenWidth / 6;
        YEnd = (i + 1) * ScreenHeight / 4;
        drawLine(XStart, YStart, XEnd, YEnd);
      }
      if (Grid[i][j].SouthWall == 1) {
        XStart = j * ScreenWidth / 6;
        YStart = (i) * ScreenHeight / 4;
        XEnd = (j + 1) * ScreenWidth / 6;
        YEnd = (i) * ScreenHeight / 4;
        drawLine(XStart, YStart, XEnd, YEnd);
      }
    }
  }
} //End of GridDraw();

void TurnRight() {
  RobotDirection++;
  if (RobotDirection == 4) {
    RobotDirection = 0;
  }
} //End of TurnRight();

void TurnLeft() {
  RobotDirection--;
  if (RobotDirection == -1) {
    RobotDirection = 3;
  }
}

void GoFwd() {
  switch (RobotDirection) {
  case 0:
    CurrentPosRow++;
    break; // Facing North
  case 1:
    CurrentPosCol++;
    break; // Facing East
  case 2:
    CurrentPosRow--;
    break; // Facing South
  case 3:
    CurrentPosCol--;
    break; // Facing West
  default:
    break;
  }

  stackPush(arrStack, RobotDirection);
  if (stackTrack != 0) {
    while ((arrStack[stackTrack - 1] - arrStack[stackTrack] == 2) || (arrStack[stackTrack - 1] - arrStack[stackTrack] == -2)) {
      stackPop(arrStack);
      stackPop(arrStack);
    }
  }
}


void DrawBot() { //Displays movement of virtual bot
  int RobotXpixelPos = 0;
  int RobotYpixelPos = 0;

  if (CurrentPosCol == 0) {
    RobotXpixelPos = ScreenWidth / 12;
  } else {
    RobotXpixelPos = (2 * CurrentPosCol + 1) * ScreenWidth / 12;
  }
  if (CurrentPosRow == 0) {
    RobotYpixelPos = ScreenHeight / 8;
  } else {
    RobotYpixelPos = (2 * CurrentPosRow + 1) * ScreenHeight / 8;
  }
  switch (RobotDirection) {
  case 0:
    displayStringAt(RobotXpixelPos, RobotYpixelPos, "^");
    break; // Facing North
  case 1:
    displayStringAt(RobotXpixelPos, RobotYpixelPos, ">");
    break; // Facing East
  case 2:
    displayStringAt(RobotXpixelPos, RobotYpixelPos, "V");
    break; // Facing South
  case 3:
    displayStringAt(RobotXpixelPos, RobotYpixelPos, "<");
    break; // Facing West
  default:
    break;
  }
}


int Solver() { //Find shortest path by right wall follow
  TurnRight();
  if (CheckWall() == 0) {
    GoFwd();
    return 0;
  }
  TurnLeft();
  if (CheckWall() == 0) {
    GoFwd();
    return 0;
  }
  TurnLeft();
  if (CheckWall() == 0) {
    GoFwd();
    return 0;
  }
  TurnLeft();
  if (CheckWall() == 0) {
    GoFwd();
    return 0;
  }
  return -1; //Should never arrive here
}


int CheckWall() { //Checks if wall in front, returns 1 if yes, 0 if no
  int WallStatus = 0;
  switch (RobotDirection) {
  case 0:
    if (Grid[CurrentPosRow][CurrentPosCol].NorthWall == 1) {
      WallStatus = 1;
    } else {
      WallStatus = 0;
    }
    break;
  case 1:
    if (Grid[CurrentPosRow][CurrentPosCol].EastWall == 1) {
      WallStatus = 1;
    } else {
      WallStatus = 0;
    }
    break;
  case 2:
    if (Grid[CurrentPosRow][CurrentPosCol].SouthWall == 1) {
      WallStatus = 1;
    } else {
      WallStatus = 0;
    }
    break;
  case 3:
    if (Grid[CurrentPosRow][CurrentPosCol].WestWall == 1) {
      WallStatus = 1;
    } else {
      WallStatus = 0;
    }
    break;
  default:
    break;
  }
  return WallStatus;
}

void DisplayStartandEnd() {
  int XpixelPos = 0;
  int YpixelPos = 0;
  if (StartPosCol == 0) {
    XpixelPos = ScreenWidth / 12;
  } else {
    XpixelPos = (2 * StartPosCol + 1) * ScreenWidth / 12;
  }
  if (StartPosRow == 0) {
    YpixelPos = ScreenHeight / 8;
  } else {
    YpixelPos = (2 * StartPosRow + 1) * ScreenHeight / 8;
  }
  displayStringAt(XpixelPos, YpixelPos, "S");
  if (TargetPosCol == 0) {
    XpixelPos = ScreenWidth / 12;
  } else {
    XpixelPos = (2 * TargetPosCol + 1) * ScreenWidth / 12;
  }
  if (TargetPosRow == 0) {
    YpixelPos = ScreenHeight / 8;
  } else {
    YpixelPos = (2 * TargetPosRow + 1) * ScreenHeight / 8;
  }
  displayStringAt(XpixelPos, YpixelPos, "E");
}

void stackPush(int * Arr, int n) {
  stackTrack++;
  Arr[stackTrack] = n;
}

void stackPop(int * Arr) {
  Arr[stackTrack] = -99;
  stackTrack--;
}

void dispenseBall() {
  playTone(300, 100);
  resetMotorEncoder(Dispenser);
  moveMotorTarget(Dispenser, 90, 40);
  waitUntilMotorStop(Dispenser);
}

void moveRobot() {
  int golfBasket = -99;
  int fwdDistance = 2460; //2460
  int wallCollide = 1200;
  bool test = true;
  int testCount = 0;


  for (int i = 0; i <= stackTrack; i++) {
    GridDraw();
    DisplayStartandEnd();
    DrawBot();
    sleep(5);


    if (test == true) {
      testCount++;
      if (arrStack[i + 2] == golfBasket) {
        test = false;
      }
    }

    if (arrStack[i + 2] == golfBasket) {
      turnRobot(arrStack[i + 1] - arrStack[i]);
      sleep(300);
      dispenseBall();
      turnRobot(2);
      i = i + 2; //Iterate to next pattern
      continue;
    }
    turnRobot(arrStack[i + 1] - arrStack[i]);
    sleep(50);
    moveForward(fwdDistance);
    switch (RobotDirection) {
    case 0:
      CurrentPosRow++;
      break; //Facing North
    case 1:
      CurrentPosCol++;
      break; //Facing East
    case 2:
      CurrentPosRow--;
      break; //Facing South
    case 3:
      CurrentPosCol--;
      break; //Facing West
    default:
      break;
    }

    if (i % 2 == 0 && CheckWall() == 1) {
      moveForward(wallCollide);
      setMotorSpeed(LeftMotor, -100);
      setMotorSpeed(RightMotor, -100);
      sleep(700);
      setMotorSpeed(LeftMotor, 0);
      setMotorSpeed(RightMotor, 0);
      sleep(100);
    }
  }
  sleep(100);
  for (int i = testCount + 2; i <= stackTrack; i++) {
    GridDraw();
    DisplayStartandEnd();
    DrawBot();
    sleep(5);


    if (arrStack[i + 2] == golfBasket) {
      turnRobot(arrStack[i + 1] - arrStack[i]);
      sleep(300);
      dispenseBall();
      turnRobot(2);
      i += 2;
      continue;
    }
    turnRobot(arrStack[i + 1] - arrStack[i]);
    sleep(50);
    moveForward(fwdDistance);

    switch (RobotDirection) {
    case 0:
      CurrentPosRow++;
      break; // Facing North
    case 1:
      CurrentPosCol++;
      break; // Facing East
    case 2:
      CurrentPosRow--;
      break; // Facing South
    case 3:
      CurrentPosCol--;
      break; // Facing West
    default:
      break;
    }

    if (CheckWall() == 1) {
      moveForward(wallCollide);
      setMotorSpeed(LeftMotor, -100);
      setMotorSpeed(RightMotor, -100);
      sleep(700);
      setMotorSpeed(LeftMotor, 0);
      setMotorSpeed(RightMotor, 0);
      sleep(100);
    }
  }
}

void turnRobot(int dir) {
  switch (dir) {
    //Turn Left
  case -1:
    moveMotorTarget(RightMotor, 1090, 60);
    moveMotorTarget(LeftMotor, 1090, -60);
    waitUntilMotorStop(RightMotor);
    waitUntilMotorStop(LeftMotor);
    TurnLeft();
    break;
  case 3:
    moveMotorTarget(RightMotor, 1090, 60);
    moveMotorTarget(LeftMotor, 1090, -60);
    waitUntilMotorStop(RightMotor);
    waitUntilMotorStop(LeftMotor);
    TurnLeft();
    break;

    //Turn 180
  case 2:
    moveMotorTarget(RightMotor, 2240, 60);
    moveMotorTarget(LeftMotor, 2240, -60);
    waitUntilMotorStop(RightMotor);
    waitUntilMotorStop(LeftMotor);
    TurnLeft();
    TurnLeft();
    break;
  case -2:
    moveMotorTarget(RightMotor, 2240, 60);
    moveMotorTarget(LeftMotor, 2240, -60);
    waitUntilMotorStop(RightMotor);
    waitUntilMotorStop(LeftMotor);
    TurnLeft();
    TurnLeft();
    break;

    //Turn Right
  case -3:
    moveMotorTarget(RightMotor, 1090, -60);
    moveMotorTarget(LeftMotor, 1090, 60);
    waitUntilMotorStop(RightMotor);
    waitUntilMotorStop(LeftMotor);
    TurnRight();
    break;
  case 1:
    moveMotorTarget(RightMotor, 1090, -60);
    moveMotorTarget(LeftMotor, 1090, 60);
    waitUntilMotorStop(RightMotor);
    waitUntilMotorStop(LeftMotor);
    TurnRight();
    break;

  default:
    break;
  }
}

void moveForward(int distance) {
  moveMotorTarget(RightMotor, distance, 100);
  moveMotorTarget(LeftMotor, distance, 100);
  waitUntilMotorStop(RightMotor);
  waitUntilMotorStop(LeftMotor);
}

/************************************END OF FUNCTIONS************************************/