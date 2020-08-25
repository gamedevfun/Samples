//! Design an elevator controller for a building of ten floors (including the ground floor).
//! Your ElevatorController class should implement the interface in ElevatorControllerBase and be
//! constructed with a reference to an ElevatorMotor object.
//! Design your controller to achive the overall-most-efficient system. The definition of overall-
//! most-efficient is up to you.


#include <cmath>
#include <chrono>
#include <queue>
#include <iostream>
#include <cassert>
#include <array>
#include <algorithm>
#include <functional>
#include <random>

//! Elevator complete requests while moving in requested direction, that fact make it efficient and logical.

class ElevatorMotor
{
public:
    ElevatorMotor()
    {
        lastTime = std::chrono::steady_clock::now();
    }
    enum Direction { Up = 1, None = 0, Down = -1 };
    int getCurrentFloor() { return static_cast<int>(std::round(currentPosition)); }
    Direction getCurrentDirection() { return currentDirection; }
    void goToFloor(int floor)
    {
        if(floor == getCurrentFloor())
            return;

        std::cout<<"Go to floor "<<floor<<std::endl;

        destinationFloor = floor;
        currentDirection = destinationFloor < getCurrentFloor() ? ElevatorMotor::Direction::Down : ElevatorMotor::Direction::Up;
    }

    void work()
    {
        auto now = std::chrono::steady_clock::now();
        auto delta = now - lastTime;
        auto floatDelta = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() * 0.001f;
        if(currentDirection == Direction::None)
            return;

        currentPosition += speed * static_cast<float>(currentDirection) * floatDelta;

        if(std::abs(static_cast<float >(destinationFloor) - currentPosition) <= 0.01f)
        {
            currentDirection = Direction::None;
            std::cout<<"stop moving on the floor "<<getCurrentFloor()<<"\n";
        }
    }
private:
    Direction currentDirection = None;
    float currentPosition = 0.0f;
    int destinationFloor = 0;
    float speed = 0.3f;

    std::chrono::steady_clock::time_point lastTime;
};

class ElevatorControllerBase
{
    // called when an up or down button was pushed on a floor
    virtual void summonButtonPushed(int summoningFloor, ElevatorMotor::Direction direction) = 0;

    // called when a button for a floor is pushed inside the car
    virtual void floorButtonPushed(int destinationFloor) = 0;

    // called when the car has reached a particular floor
    virtual void reachedFloor(int floor) = 0;
};

template<size_t _FLOOR_COUNT>
class ElevatorController : public ElevatorControllerBase
{
    struct FloorRequest
    {
        int floor = 0;
        ElevatorMotor::Direction direction = ElevatorMotor::Direction::None;
    };

    struct QueuedRequest
    {
        QueuedRequest(bool isDestination, FloorRequest* request)
        : isDestination()
        , request(request)
        {
        }
        bool isDestination = false;
        ElevatorMotor::Direction direction = ElevatorMotor::Direction::None;
        FloorRequest* request;
    };

public:
    explicit ElevatorController(ElevatorMotor* motor)
    : motor(motor)
    {
        assert(_FLOOR_COUNT > 1);
        lastFloor = motor->getCurrentFloor();
    }

    // called when an up or down button was pushed on a floor
    void summonButtonPushed(int summoningFloor, ElevatorMotor::Direction direction) override
    {
        std::cout<<"Summon btn pushed floor - "<<summoningFloor<<" direction - "<<direction<<std::endl;

        //! do not make request if already have one
        auto it = std::find_if(requestQueue.begin(), requestQueue.end(), [=] (QueuedRequest r)
        {
            return r.request->floor == summoningFloor && r.direction == direction;
        });

        if(it != requestQueue.end())
            return;

        auto request = MakeRequest(summoningFloor, direction);
        requestQueue.emplace_back(false, request);
        requestQueue.back().direction = direction;
    }

    // called when a button for a floor is pushed inside the car
    void floorButtonPushed(int destinationFloor) override
    {
        std::cout<<"Floor btn pushed floor - "<<destinationFloor<<std::endl;

        if(destinationFloor == motor->getCurrentFloor())
            return;

        //! do not make request if already have one
        auto it = std::find_if(requestQueue.begin(), requestQueue.end(), [=] (QueuedRequest r)
        {
            return r.request->floor == destinationFloor;
        });

        if(it != requestQueue.end())
            return;

        auto direction = destinationFloor < motor->getCurrentFloor() ? ElevatorMotor::Direction::Down : ElevatorMotor::Direction::Up;
        auto request = MakeRequest(destinationFloor, direction);
        requestQueue.emplace_back(true, request);
    }

    // called when the car has reached a particular floor
    void reachedFloor(int floor) override
    {
        std::cout<<"Reached floor - "<<floor<<std::endl;
        auto request = requestedFloors[floor];
        if(request == nullptr)
            return;

        if(request->direction == motor->getCurrentDirection()
            || isBothDirectionRequest(request)
            || request == currentRequest)
        {
            if(request == currentRequest)
                currentRequest = nullptr;

            TryRemoveQueuedRequest(request);

            if(!isBothDirectionRequest(request) || floor == 0 || floor == _FLOOR_COUNT-1)
            {
                requestedFloors[floor] = nullptr;
                delete request;
                RemoveQueuedRequest(request);
                requestCount--;
                std::cout<<"FloorRequest removed. Count - "<<requestCount<<std::endl;
            }
            else
                request->direction = getOppositeDirection(motor->getCurrentDirection());

            std::cout<<"Complete move to requested floor "<<floor<<std::endl;
            if(onRequestCompleted)
                onRequestCompleted();
        }
    }

    void work()
    {
        if(motor->getCurrentDirection() == ElevatorMotor::Direction::None && !requestQueue.empty())
        {
            auto request = requestQueue.front();
            requestQueue.pop_front();
            motor->goToFloor(request.request->floor);
        }

        motor->work();
        if(motor->getCurrentFloor() != lastFloor)
        {
            lastFloor = motor->getCurrentFloor();
            reachedFloor(lastFloor);
        }
    }

    void subscribeRequestCompleted(const std::function<void()>& callBack)
    {
        onRequestCompleted = callBack;
    }

private:

    FloorRequest* MakeRequest(int floor, ElevatorMotor::Direction direction)
    {
        auto request = requestedFloors[floor];
        if(request == nullptr)
        {
            request = new FloorRequest {floor, direction };
            requestedFloors[floor] = request;
            requestCount++;
            std::cout<<"FloorRequest created. Count - "<<requestCount<<std::endl;
        }
        else if(request->direction != direction)
        {
            // Both direction request on the floor
            request->direction = ElevatorMotor::Direction::None;
        }

        return request;
    }

    void TryRemoveQueuedRequest(FloorRequest* request)
    {
        requestQueue.erase(std::remove_if(requestQueue.begin(), requestQueue.end(), [=] (const QueuedRequest& r)
        {
            return r.request->floor == request->floor && (r.direction == motor->getCurrentDirection()
                                                          || r.isDestination
                                                          || request->floor == 0 || request->floor == _FLOOR_COUNT-1);
        }), requestQueue.end());
    }

    void RemoveQueuedRequest(FloorRequest* request)
    {
        requestQueue.erase(std::remove_if(requestQueue.begin(), requestQueue.end(), [=] (const QueuedRequest& r)
        {
            return r.request == request;
        }), requestQueue.end());
    }

    ElevatorMotor::Direction getOppositeDirection(ElevatorMotor::Direction direction)
    {
        if(direction == ElevatorMotor::Direction::Down)
            return ElevatorMotor::Direction::Up;
        if(direction == ElevatorMotor::Direction::Up)
            return ElevatorMotor::Direction::Down;
        return direction;
    }

    bool isBothDirectionRequest(FloorRequest* request)
    {
        return request->direction == ElevatorMotor::Direction::None;
    }

    ElevatorMotor* motor;
    int lastFloor;
    //! queue for request
    std::deque<QueuedRequest> requestQueue;
    //! requestedFloors for fast check if any request on reached floor that we can complete while moving
    std::array<FloorRequest*, _FLOOR_COUNT> requestedFloors = {nullptr};
    FloorRequest* currentRequest = nullptr;
    std::function<void()> onRequestCompleted;
    int requestCount = 0;
};


int main()
{
    ElevatorMotor motor;
    ElevatorController<10> controller(&motor);

    std::default_random_engine rGen;
    std::uniform_int_distribution<int> rand(0, 9);

    controller.subscribeRequestCompleted([&] () {
        controller.floorButtonPushed(rand(rGen));
    });
    controller.summonButtonPushed(3, ElevatorMotor::Direction::Up);
    controller.summonButtonPushed(5, ElevatorMotor::Direction::Up);
    controller.summonButtonPushed(6, ElevatorMotor::Direction::Down);
    controller.summonButtonPushed(0, ElevatorMotor::Direction::Up);

    while (true)
    {
        controller.work();
    }

    return 0;
}
