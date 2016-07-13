 ##FlappyBirdAI
 AI play FlappyBird via Machine Learning and OpenCV
  
 ##Trying it out
 1. run FlappyBirdAI.exe
 2. visit: FlappyBird webpage version [online-link](http://ben7th.github.io/flappy-html5-bird/) or [offline(gamebin/index.htm)](gamebin/index.htm)
 3. Move mouse pointer above FlappyBird to make AI auto-control start
 
 ##How it works
 1. Take snapshots of the game graphics via Windows API
 2. Recognize bird and pipes in snapshots via OpenCV
 3. Obtain states of bird by watcher classes within State Machine
 4. Predicate contact-time via Linear Regression
 5. Auto click the mouse to make bird jump via Windows API
 6. simple Self-Training to get higher scores
 
 **Keywords: OpenCV, State Machine, Linear Regression**
 
 ##Runtime Requirements
 Windows XP, 7+
 
 ##Build Requirements
 Windows 7+
 VS 2012+
