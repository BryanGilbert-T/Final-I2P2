// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Sheets.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/IntroScene.h"
#include "Scene/Boarding.hpp"
#include "Scene/friendlist.hpp"
#include "Scene/Login.hpp"
#include "Scene/requests.hpp"
#include "Scene/search.hpp"
#include "Scene/Signup.hpp"

#include "fstream"
#include "Scene/leaderboard.hpp"
#include "Scene/Story.hpp"

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    // TODO HACKATHON-2 (2/3): Register Scenes here
    game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("start", new StartScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("intro", new IntroScene());
	game.AddNewScene("boarding", new BoardingScene());
	game.AddNewScene("login", new LoginScene());
	game.AddNewScene("signup", new SignupScene());
	game.AddNewScene("friendlist", new FriendListScene());
	game.AddNewScene("requests", new RequestsScene());
	game.AddNewScene("search", new SearchScene());
	game.AddNewScene("leaderboard", new LeaderboardScene());
	game.AddNewScene("story-1", new Story(1, 0.5f, 5));
	game.AddNewScene("story-2", new Story(4, 0.5f, 5));

	std::string curUser;
	std::ifstream in("Resource/account.txt");
	if (in.peek() != EOF) {
		in >> curUser;
		set_online(curUser, true);
	}

    // TODO HACKATHON-1 (1/1): Change the start scene
	game.Start("intro", 60, 1920, 1080);
	return 0;
}
