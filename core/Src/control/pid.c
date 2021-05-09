float calculateProportional(float currentPosition, float setpoint, PIDProfile profile, double deltaTime) {
    // Calculate error
    float error = setpoint - currentPosition;
    // Calculate correction and return
    return error * profile.kP + profile.kF;
}