#pragma once

#include "Data.h"

#include <Arduboy2.h>

class Demo
{
        public:

            Arduboy2 arduboy;

            void setup()
            {
                this->arduboy.begin();
                this->ball.y = ball.ballSpawn;
            }

            void loop()
            {
                if(!this->arduboy.nextFrame())
			        return;

			    this->arduboy.pollButtons();
			    this->arduboy.clear();
                this->GameLoop();
			    this->arduboy.display();
            }

        public:

            struct Ball
            {
                float y;
                float x = 56;
                float yVelocity;

                float ballSpawn = 22;
                float newBallSpawn;

                float translateBallSpeed = 0.5;

                uint8_t width = 16;
                uint8_t height = 16;
            };

            struct Physics
            {
                float gravity = 0.1;
                float accelerationModifier = 0.03;
                float bounceVelocity = 0;
            };

            struct Vars
            {
                bool printAsteriskG;
                bool printAsteriskA;
            };

            enum class State : uint8_t 
            {
                Preview,
                Simulation
            };

            Ball ball;
            Physics physics;
            Vars vars;
            State state;

        public:

            void GameLoop()
            {
                //state machine
                switch (this->state)
                {
                    case State::Preview:
                        this->previewLogic();
                            break;
                    case State::Simulation:
                        this->simulationLogic();
                            break;  
                }

                //should the asterisk appear after the value
                this->vars.printAsteriskG = (this->physics.gravity == 0.1) ? true : false;
                this->vars.printAsteriskA = (this->physics.accelerationModifier == 0.03) ? true : false;

                this->draw();
            }

            void previewLogic()
            {
                this->arduboy.setCursor(0, 0);
                this->arduboy.print(F("Preview"));

                if (this->arduboy.justPressed(B_BUTTON))
                {
                    this->state = State::Simulation;
                    this->ball.newBallSpawn = this->ball.y;
                }

                //ball height control
                if (this->arduboy.pressed(A_BUTTON) && this->ball.y > 0)
                {
                    if (this->arduboy.pressed(UP_BUTTON))
                    { 
                        this->ball.y -= this->ball.translateBallSpeed;
                    }
                }

                if (this->arduboy.pressed(A_BUTTON) && this->ball.y < 38)
                {
                    if (this->arduboy.pressed(DOWN_BUTTON))
                    {
                        this->ball.y += this->ball.translateBallSpeed;
                    }
                }

                //gravity and acceleration button control
                if (this->arduboy.justPressed(UP_BUTTON) && this->arduboy.notPressed(A_BUTTON))
                {
                    this->physics.gravity += 0.01;
                }

                if (this->arduboy.justPressed(DOWN_BUTTON) && this->arduboy.notPressed(A_BUTTON))
                {
                    this->physics.gravity -= 0.01;
                }

                if (this->arduboy.justPressed(RIGHT_BUTTON))
                {
                    this->physics.accelerationModifier += 0.01;
                }

                if (this->arduboy.justPressed(LEFT_BUTTON))
                {
                    this->physics.accelerationModifier -= 0.01;
                }

            }

            void simulationLogic()
            {
                this->arduboy.setCursor(0, 0);
                this->arduboy.print(F("Simulation"));

                if (this->arduboy.justPressed(B_BUTTON))
                {
                    this->state = State::Preview;
                    this->ball.y = ball.newBallSpawn;
                    this->ball.yVelocity = 0;
                }

                //simulating gravity
                this->ball.yVelocity += this->physics.gravity;

                //adding velocity to the position
                this->ball.y += this->ball.yVelocity;

                Rect ballHitbox(this->ball.x, this->ball.y, this->ball.width, this->ball.height);
                Rect trampolineHitbox(48, 57, 32, 7);

                //collision
                if (this->arduboy.collide(ballHitbox, trampolineHitbox))
                {
                    this->physics.bounceVelocity = this->ball.yVelocity;
                    this->ball.yVelocity = 0;
                    this->ball.yVelocity -= this->physics.bounceVelocity; 
                }
            }

            void draw()
            {
                //drawing
                Sprites::drawExternalMask(48, 57, trampolineSprite, trampolineMask, 0, 0);
                Sprites::drawExternalMask(this->ball.x, this->ball.y, ballSprite, ballMask, 0, 0);

                //printing
                this->arduboy.setTextColor(WHITE);
                this->arduboy.setCursor(0, 8);
                this->arduboy.print(F("A:"));
                this->arduboy.print(this->physics.accelerationModifier);

                if (this->vars.printAsteriskA)
                {
                    this->arduboy.setTextColor(WHITE);
                    this->arduboy.print(F("*"));
                }
                else
                {
                    this->arduboy.setTextColor(BLACK);
                    this->arduboy.print(F("*"));
                }

                this->arduboy.setTextColor(WHITE);
                this->arduboy.setCursor(0, 16);
                this->arduboy.print(F("G:"));
                this->arduboy.print(this->physics.gravity);
                
                if (this->vars.printAsteriskG)
                {
                    this->arduboy.setTextColor(WHITE);
                    this->arduboy.print(F("*"));
                }
                else
                {
                    this->arduboy.setTextColor(BLACK);
                    this->arduboy.print(F("*"));
                }

                this->arduboy.setTextColor(WHITE);
                this->arduboy.setCursor(86, 0);
                this->arduboy.print(F("V:"));
                this->arduboy.print(this->ball.yVelocity);

                this->arduboy.setTextColor(WHITE);
                this->arduboy.setCursor(86, 8);
                this->arduboy.print(F("P:"));
                this->arduboy.print(this->ball.y);
            }
};          
