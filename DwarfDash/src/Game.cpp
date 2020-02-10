class Game {

public:
	Game() {};
	~Game() {};

	void init() {
		
	}

	void update() {

	}

	void draw() {

	}

	void reset() {

	}

private:
	//std::vector<Gameobject*> objects;

	int const GAME_STATE_IDLE = 0;
	int const GAME_STATE_ACTIVE = 1;
	int const GAME_STATE_PAUSED = 2;
	int const GAME_STATE_FINISHED = 3;

	int activeGameState = GAME_STATE_IDLE;

};