
#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatsObject.h"
#include "ExplosionObject.h"
#include "TextObject.h"
#include "PlayerPower.h"
#include "Geometric.h"
#include "BossObject.h"

#define USE_AUDIO
BaseObject g_background;
TTF_Font* g_font_text = NULL;

Mix_Chunk *g_sound_bullet = NULL;
Mix_Chunk *g_sound_explosion = NULL;
Mix_Chunk *g_sound_ex_main = NULL;
Mix_Music *g_music = NULL;

bool InitData()
{
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if(ret < 0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    g_window = SDL_CreateWindow("SONIC DAI CHIEN QUAI VAT",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if(g_window == NULL )
    {
        success = false;
    }
    else
    {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL)
            success = false;
        else
        {
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags))
                success = false;
        }

        if (TTF_Init() == -1)
        {
            success = false;
        }

        //Initialize SDL_mixer
        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
        {
            printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        g_font_text = TTF_OpenFont("font//dlxfont.ttf", 15);
        if (g_font_text == NULL)
        {
            return false;
        }
    }


    return success;
}

bool LoadAudio()
{
    bool success = true;

    g_sound_bullet = Mix_LoadWAV( "sound//bullet.wav" );
    if( g_sound_bullet == NULL )
    {
        printf( "Failed to load bullet sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }

    g_sound_explosion = Mix_LoadWAV( "sound//explosion.wav" );
    if( g_sound_explosion == NULL )
    {
        printf( "Failed to load explosion sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }

    g_sound_ex_main = Mix_LoadWAV( "sound//die.wav" );
    if( g_sound_ex_main == NULL )
    {
        printf( "Failed to load main die sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }

     g_music = Mix_LoadMUS("sound//background.mp3");
     if (g_music == NULL) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
     }
     if (g_music != NULL) {
        Mix_PlayMusic(g_music, -1);
    }

    return success;
}

void close()
{
    g_background.Free();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    IMG_Quit();
    SDL_Quit();
}

bool LoadBackground()
{
    bool ret = g_background.LoadImg("img//background.png", g_screen);
    if (ret == false)
    {
        return false;
    }

    return true;
}

std::vector<ThreatsObject*> MakeThreadList()
{

    std::vector<ThreatsObject*> list_threats;

    ThreatsObject* thread_objs = new ThreatsObject[20];

    for (int i = 0; i < 20; i++)
    {
        ThreatsObject* thread_obj = (thread_objs+i);
        if (thread_obj != NULL)
        {
            thread_obj->LoadImg("img//threat_left.png", g_screen);
            thread_obj->set_clips();
            thread_obj->InitPlayer();
            thread_obj->set_xpos(500 + i*500);
            thread_obj->set_ypos(200);
            thread_obj->set_type_move(ThreatsObject::MOVE_IN_SPACE_TH);
            int pos1 = thread_obj->get_x_pos() - 60;
            int pos2 = thread_obj->get_x_pos() + 60;
            thread_obj->SetAnimationPos(pos1, pos2);
            list_threats.push_back(thread_obj);
        }
    }

    ThreatsObject* thread_objs2 = new ThreatsObject[15];
    for (int i = 0; i < 15; i++)
    {
        ThreatsObject* thread_obj3 = (thread_objs2+i);
        if (thread_obj3 != NULL)
        {
            thread_obj3->LoadImg("img//threat_level.png", g_screen);
            thread_obj3->set_clips();
            thread_obj3->InitPlayer();
            thread_obj3->set_input_left(0);
            thread_obj3->set_xpos(700 + i*1500);
            thread_obj3->set_ypos(250);

            thread_obj3->set_type_move(ThreatsObject::STATIC_TH);

            BulletObject* p_bullet = new BulletObject();
            thread_obj3->InitBullet(p_bullet, g_screen);
            list_threats.push_back(thread_obj3);
        }
    }


    return list_threats;
}
// MENU
bool showMenu(SDL_Renderer* renderer)
{
    SDL_Texture* menuTexture = IMG_LoadTexture(renderer, "img//menu.jpg");
    if (!menuTexture)
    {
        SDL_Log("Không thể tải hình ảnh menu: %s", IMG_GetError());
        return false;
    }

    SDL_Rect destRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menuTexture, NULL, &destRect);

    TTF_Font* font = TTF_OpenFont("font//dlxfont.ttf", 36);
    SDL_Color whiteColor = { 255, 255, 255 };

    TextObject playText;
    playText.SetText("PLAY GAME");
    playText.loadFromRenderedText(font, renderer);
    SDL_Rect playButton = { 100, SCREEN_HEIGHT / 2 - 50, 160, 80 };
playText.RenderText(renderer, playButton.x + 20, playButton.y + 10);

    TextObject exitText;
    exitText.SetText("EXIT");
    exitText.loadFromRenderedText(font, renderer);
    SDL_Rect exitButton = { 100, SCREEN_HEIGHT / 2 + 50, 160, 80 };
exitText.RenderText(renderer, exitButton.x + 50, exitButton.y + 10);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (true)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_DestroyTexture(menuTexture);
                return false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (x >= playButton.x && x <= playButton.x + playButton.w &&
                    y >= playButton.y && y <= playButton.y + playButton.h)
                {
                    SDL_DestroyTexture(menuTexture);
                    return true; // Bắt đầu game
                }
                else if (x >= exitButton.x && x <= exitButton.x + exitButton.w &&
                         y >= exitButton.y && y <= exitButton.y + exitButton.h)
                {
                    SDL_DestroyTexture(menuTexture);
                    return false; // Thoát game
                }
            }
        }
    }
}

//GAME OVER
void showGameOverScreen(SDL_Renderer* renderer, bool &restartGame)
{
    SDL_Texture* gameOverTexture = IMG_LoadTexture(renderer, "img//game_over.jpg");
    if (!gameOverTexture)
    {
        SDL_Log("khong the tai hinh anh Game Over: %s", IMG_GetError());
        return;
    }

    SDL_Rect destRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &destRect);

    TTF_Font* font = TTF_OpenFont("font//dlxfont.ttf", 36);
    SDL_Color whiteColor = { 255, 255, 255 };

    TextObject playAgainText;
    playAgainText.SetText("PLAY AGAIN");
    playAgainText.loadFromRenderedText(font, renderer);
    int playAgainTextWidth = playAgainText.getWidth();
    playAgainText.RenderText(renderer, (SCREEN_WIDTH - playAgainTextWidth) / 2, SCREEN_HEIGHT / 2 + 50);


    SDL_Rect yesButton = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 130, 80, 40 };  // Moved down more
    TextObject yesText;
    yesText.SetText("YES");
    yesText.loadFromRenderedText(font, renderer);
    int yesTextWidth = yesText.getWidth();
    yesText.RenderText(renderer, yesButton.x + (yesButton.w - yesTextWidth) / 2 , yesButton.y + 5);

    // Hiển thị chữ "NO"
   SDL_Rect noButton = { SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 + 130, 80, 40 };  //Moved down more
    TextObject noText;
    noText.SetText("NO");
    noText.loadFromRenderedText(font, renderer);
     int noTextWidth = noText.getWidth();
    noText.RenderText(renderer, noButton.x + (noButton.w - noTextWidth) / 2 , noButton.y + 5);


    SDL_RenderPresent(renderer);

    SDL_Event e;
    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
                restartGame = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x >= yesButton.x && x <= yesButton.x + yesButton.w &&
                    y >= yesButton.y && y <= yesButton.y + yesButton.h)
                {
                    restartGame = true;  // Chơi lại từ đầu
                    running = false;
                }
                else if (x >= noButton.x && x <= noButton.x + noButton.w &&
                         y >= noButton.y && y <= noButton.y + noButton.h)
                {
                    restartGame = false; // Thoát game
                    running = false;
                    SDL_Quit();  // Thoát SDL
                    exit(0);      // Thoát chương trình
                }
            }
        }
    }

    SDL_DestroyTexture(gameOverTexture);
    TTF_CloseFont(font);  // Don't forget to close the font!
}
// Phát nhạc nền

int main(int argc, char* argv[])
{
    ImpTimer fps;
    if (InitData() == false)
    {
        return -1;
    }
    if(!LoadAudio()) { //Load Audio
       return -1;
    }

    if (!showMenu(g_screen))
    {
        close();
        return 0;
    }

    if (!LoadBackground())
    {
        return -1;
    }

    GameMap game_map;
    game_map.LoadMap("map//map01.dat");
    game_map.LoadMapTiles(g_screen);

    MainObject p_player;
    p_player.LoadImg(g_name_main_right, g_screen);
    p_player.set_clips();


    PlayerPower player_power;
    player_power.Init(g_screen);


    PlayerMoney player_money;
    player_money.Init(g_screen);
    player_money.SetPos(SCREEN_WIDTH*0.5 - 300, 8);


    std::vector<ThreatsObject*> threats_list = MakeThreadList();

    BossObject bossObject;
    bossObject.LoadImg("img//boss_object.png", g_screen);
    bossObject.set_clips();
    int xPosBoss = MAX_MAP_X*TILE_SIZE - SCREEN_WIDTH*0.6;
    bossObject.set_xpos(xPosBoss);
    bossObject.set_ypos(10);

    ExplosionObject exp_threats;
    ExplosionObject exp_main;

    bool ret = exp_threats.LoadImg("img//exp3.png", g_screen);
    exp_threats.set_clips();
    if (!ret) return -1;


    ret = exp_main.LoadImg("img//exp3.png", g_screen);
    exp_main.set_clips();
    if (!ret) return 0;


    TextObject time_game;
    time_game.setColor(TextObject::WHITE_TEXT);

    TextObject mark_game;
    mark_game.setColor(TextObject::WHITE_TEXT);

    TextObject money_count;
    money_count.setColor(TextObject::WHITE_TEXT);

    int num_die = 0;
    unsigned int mark_value = 0;

    bool quit = false;
    while(!quit)
    {

        fps.start();
        while( SDL_PollEvent(&g_event) != 0 )
        {
            //User requests quit
            if(g_event.type == SDL_QUIT )
            {
                quit = true;
            }

            p_player.HandleInputAction(g_event, g_screen);
        }

        SDL_RenderClear(g_screen);

        g_background.Render(g_screen, NULL);

        Map ga_map = game_map.GetMap();

        p_player.HandleBullet(g_screen);
        p_player.SetMapXY(ga_map.start_x_, ga_map.start_y_);
        p_player.DoPlayer(ga_map);
        p_player.Show(g_screen);

        game_map.SetMap(ga_map);
       game_map.DrawMap(g_screen);

       //Draw Geometric
       GeometricFormat rectange_size(0, 0, SCREEN_WIDTH, 40);
       ColorData color_data(36, 36, 36);
       Gemometric::RenderRectange(rectange_size, color_data, g_screen);

       GeometricFormat outlie_size(1, 1, SCREEN_WIDTH - 1, 38);
       ColorData color_data1(255, 255, 255);
       Gemometric::RenderOutline(outlie_size, color_data1, g_screen);




       player_power.Show(g_screen);
       player_money.Show(g_screen);

       for (int i = 0; i < threats_list.size(); i++)
       {
           ThreatsObject* obj_threat = threats_list.at(i);
           obj_threat->SetMapXY(ga_map.start_x_, ga_map.start_y_);
           obj_threat->ImpMoveType(g_screen);
           obj_threat->DoPlayer(ga_map);
           obj_threat->Show(g_screen);
           obj_threat->MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);


           SDL_Rect rect_player = p_player.GetRectFrame();

           bool is_col1 = false;
           std::vector<BulletObject*> bullet_list = obj_threat->get_bullet_list();
           for (int am = 0; am < bullet_list.size(); am++)
           {
               BulletObject* p_bullet = bullet_list.at(am);
               if (p_bullet)
               {
                   is_col1 = SDLCommonFunc::CheckCollision(p_bullet->GetRect(), rect_player);
                   if (is_col1 == true)
                   {
                       obj_threat->ResetBullet(p_bullet);
                       break;
                   }
               }
           }

           SDL_Rect rect_threat = obj_threat->GetRectFrame();
           bool is_col2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);

           if (is_col2 || is_col1)
           {
               int width_exp_frame = exp_main.get_frame_height();
               int heiht_exp_height = exp_main.get_frame_width();
               for (int ex = 0; ex < 4; ex++)
               {
                   int x_pos = (p_player.GetRect().x + p_player.get_frame_width()*0.5) - width_exp_frame*0.5;
                  int y_pos = (p_player.GetRect().y + p_player.get_frame_height()*0.5) - heiht_exp_height*0.5;

                   exp_main.set_frame(ex);
                   exp_main.SetRect(x_pos, y_pos);
                   exp_main.Show(g_screen);
                   SDL_RenderPresent(g_screen);
               }
#ifdef USE_AUDIO
               Mix_PlayChannel(-1, g_sound_ex_main, 0);
#endif
               num_die++;
               if (num_die <= 3)
               {
                   p_player.SetRect(0, 0);
                   p_player.set_think_time(60);
                   SDL_Delay(1000);
                   player_power.Decrease();
                   player_power.Render(g_screen);
                   continue;
               }
               else
               {

                   bool restartGame = false;
showGameOverScreen(g_screen, restartGame);
if (restartGame)
{
    num_die = 0;
    mark_value = 0;

    p_player.SetRect(0, 0);
    p_player.set_think_time(0);
    player_power.Init(g_screen);
    player_money.Init(g_screen);

    threats_list = MakeThreadList();
    game_map.LoadMap("map//map01.dat");

    continue;
}
else
{
    obj_threat->Free();
    close();
    SDL_Quit();
    return 0;
}

               }
           }
       }

       std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();
       int frame_exp_width = exp_threats.get_frame_width();
       int frame_exp_height = exp_threats.get_frame_width();

       for (int am = 0; am < bullet_arr.size(); am++)
       {
           BulletObject* p_bullet = bullet_arr.at(am);
           if (p_bullet)
           {

               for (int i = 0; i < threats_list.size(); i++)
               {
                   ThreatsObject* obj_threat = threats_list.at(i);

                   SDL_Rect threat_rect;
                   threat_rect.x = obj_threat->GetRect().x;
                   threat_rect.y = obj_threat->GetRect().y;
                   threat_rect.w = obj_threat->get_width_frame();
                   threat_rect.h = obj_threat->get_height_frame();

                   bool is_col = SDLCommonFunc::CheckCollision(p_bullet->GetRect(), threat_rect);
                   if (is_col)
                   {
                       mark_value++;
                       for (int ex = 0; ex < 8; ex++)
                       {
                           int x_pos = p_bullet->GetRect().x - frame_exp_width*0.5;
                           int y_pos = p_bullet->GetRect().y - frame_exp_height*0.5;

                           exp_threats.set_frame(ex);
                           exp_threats.SetRect(x_pos, y_pos);
                           exp_threats.Show(g_screen);
                       }
                       p_player.RemoveBullet(am);


                           obj_threat->Free();
                           threats_list.erase(threats_list.begin() + i);

#ifdef USE_AUDIO
     Mix_PlayChannel(-1, g_sound_ex_main, 0);
#endif
                   }
               }
           }
       }

       std::string str_time = "Time: ";
       Uint32 time_val = SDL_GetTicks() / 1000;
       Uint32 val_time = 300 - time_val;

       if (val_time <= 0)
       {
           if (MessageBoxW(NULL, L"GAME OVER", L"Info", MB_OK | MB_ICONSTOP) == IDOK)
           {
               quit = true;
               break;
           }
       }
       else
       {
           std::string str_val = std::to_string(val_time);
           str_time += str_val;

           time_game.SetText(str_time);
           time_game.loadFromRenderedText(g_font_text, g_screen);
           time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
       }
       std::string val_str_mark = std::to_string(mark_value);
       std::string strMark("Mark: ");
       strMark += val_str_mark;

       mark_game.SetText(strMark);
       mark_game.loadFromRenderedText(g_font_text, g_screen);
       mark_game.RenderText(g_screen, SCREEN_WIDTH*0.5 - 50, 15);

       int money_val = p_player.GetMoneyCount();
       std::string money_count_str = std::to_string(money_val);
       money_count.SetText(money_count_str);
       money_count.loadFromRenderedText(g_font_text, g_screen);
       money_count.RenderText(g_screen, SCREEN_WIDTH*0.5 - 250, 15);

       //Process Boss
       int val = MAX_MAP_X*TILE_SIZE - (ga_map.start_x_ + p_player.GetRect().x);
       if (val <= SCREEN_WIDTH)
       {
        bossObject.SetMapXY(ga_map.start_x_, ga_map.start_y_);
        bossObject.DoPlayer(ga_map);
        bossObject.MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
        bossObject.Show(g_screen);
       }

        //Update screen
        SDL_RenderPresent(g_screen);

        int imp_time = fps.get_ticks();
        int time_for_one_frame = 1000/FRAMES_PER_SECOND;
        if(imp_time < time_for_one_frame)
        {
            SDL_Delay(time_for_one_frame - imp_time);
        }

    }


    for (int i = 0; i < threats_list.size(); i++)
    {
        ThreatsObject* p_thread = threats_list.at(i);
        p_thread->Free();
        p_thread = NULL;
    }

    threats_list.clear();

    close();

    return 0;
}

