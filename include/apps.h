#ifndef APPS_H
#define APPS_H

class App;
struct renderer;

App *initializeApp(renderer *rnd, const char *name);

#include <renderer.h>

class App {
public:
    explicit App(renderer *rnd);
    virtual ~App() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void destroy() = 0;

protected:
    renderer *rnd;
};



#endif //APPS_H
