const int SDL_WINDOW_SIZE = 600;
const int AGENT_SIZE = 10;
const int AGENT_AMOUNT = 70;
const int TURN_SPEED = 5; // larger values are slower turns
const int MOVE_SPEED = 1;
const int DEVIATION_ANGLE = 25;

const int ALIGNMENT_RADIUS = 25;
const double ALIGNMENT_STRENGTH = 0.3;

const int AVOID_RADIUS = 25;
const double AVOID_STRENGTH = 0.6;

const int COHERENCE_RADIUS = 35;
const double COHERENCE_STRENGTH = 0.2;

const int OBSTACLE_RADIUS = 55;
const double OBSTACLE_STRENGTH = 0; // avoidance of obstacles (currently only the map edges.)