class FPScounter
{
      private:
              int startTicks;
              bool started;
      public:
             FPScounter();
             int get_ticks(){if(started == true){return SDL_GetTicks() - startTicks;}return 0;}
             void startcount(){started = true; startTicks = SDL_GetTicks();}
             void stop(){started = false;}
             bool is_started(){return started;}
};

FPScounter::FPScounter()
{
    //Initialize the variables
    startTicks = 0;
    started = false;
}
