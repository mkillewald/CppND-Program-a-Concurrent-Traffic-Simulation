#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

template <class T> class MessageQueue {
public:
  T receive();
  void send(T &&msg);

private:
  std::mutex _mutex;
  std::condition_variable _cond;
  std::deque<T> _queue;
};

enum TrafficLightPhase { red, green };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();

  // getters / setters
  TrafficLightPhase getCurrentPhase();
  void setCurrentPhase(TrafficLightPhase phase);

  // typical behaviour methods
  void waitForGreen();
  void simulate();

private:
  // typical behaviour methods
  void cycleThroughPhases();

  MessageQueue<TrafficLightPhase> _messages;
  std::condition_variable _condition;
  std::mutex _mutex;
  TrafficLightPhase _currentPhase;
};

#endif