Chassis calculateMecanum(float xThrottle, float yThrottle, float rotationThrottle) {
		// Calculate the power for each wheel (on a scale of -1.0 to 1.0)
		float frontRight = xThrottle - yThrottle - rotationThrottle;
		float frontLeft = xThrottle + yThrottle + rotationThrottle;
		float backRight = xThrottle + yThrottle - rotationThrottle;
		float backLeft = xThrottle - yThrottle + rotationThrottle;

		// Find the highest magnitude (max) value in the four power variables.
		float max = abs(frontRight);
		if (abs(frontLeft) > max) max = abs(frontLeft);
		if (abs(backRight) > max) max = abs(backRight);
		if (abs(backLeft) > max) max = abs(backLeft);

		// If the max value is greater than 1.0, divide all the motor power variables by the max value, forcing all magnitudes to be less than or equal to 1.0.
		if (max > 1.0) {
				frontRight /= max;
				frontLeft /= max;
				backRight /= max;
				backLeft /= max;
		}

      Chassis chassis;
      chassis.frontRight = frontRight;
      chassis.frontLeft = frontLeft;
      chassis.backRight = backRight;
      chassis.backLeft = backLeft;
}