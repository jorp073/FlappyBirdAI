##FlappyBirdAI
AI play FlappyBird via Machine Learning and OpenCV

##Trying it out
1. run xxxx(todo)
2. visit: [FlappyBird webpage version](http://ben7th.github.io/flappy-html5-bird/)
3. Move mouse pointer onto FlappyBird to start AI auto-controlling

##How it works
1. Take snapshots of the game graphics via Windows API
2. Recognize bird and pipes in snapshots via OpenCV
3. Obtain states of bird by watchers within State Machine
4. Predicate contact-time via Machine Learning
5. Auto click the mouse to make bird jump via Windows API
6. Self-Training to get higher scores

**Keywords: OpenCV, State Machine, Linear Regression**

##Runtime Requirements
Windows XP, 7+

##Build Requirements
VS 2012+