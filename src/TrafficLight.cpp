#include "TrafficLight.h"
#include <future>
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> uLock(_mutex);
  _cond.wait(uLock, [this] { return !_queue.empty(); });
  T msg = std::move(_queue.back());
  _queue.pop_back();
  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> uLock(_mutex);
  _queue.push_back(std::move(msg));
  _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }
void TrafficLight::setCurrentPhase(TrafficLightPhase phase) {
  _currentPhase = phase;
}

void TrafficLight::waitForGreen() {
  while (_messages.receive() == TrafficLightPhase::red) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void TrafficLight::simulate() {
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int64_t> dist(6, 10);
  auto randomDuration = dist(mt);

  std::chrono::time_point<std::chrono::steady_clock> cycleStart =
      std::chrono::steady_clock::now();

  while (true) {

    auto timeSinceStart = std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::steady_clock::now() - cycleStart)
                              .count();
    if (timeSinceStart >= randomDuration) {
      getCurrentPhase() == red ? setCurrentPhase(green) : setCurrentPhase(red);
      _messages.send(std::move(getCurrentPhase()));

      cycleStart = std::chrono::steady_clock::now();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}