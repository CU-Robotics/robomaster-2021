#include <subsystems/Subsystem.hpp>

class Robot : buffs-robomaster-2021::lib::robots {
    //class description
    public:
        Robot(int id);
        void addSubsystem(Subsystem subsystem);

    private:
        Subsystem[] subsystems;
}