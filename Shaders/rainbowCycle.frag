#version 460 core

out vec4 FragColor;

uniform float time;

const float PI = 3.14159265359;

// n will be used to define the periodicy of the function
int n;
float rainbow_R(float seconds);
float rainbow_G(float seconds);
float rainbow_B(float seconds);

void main()
{
	if (time == 0)
		n = 0;
	else
		n = int(time/6.0) * 6;

	FragColor = vec4(rainbow_R(time), rainbow_G(time), rainbow_B(time), 1.0);
	//FragColor = vec4(rainbow_R(time), 0.0, 0.0, 1.0);
	//FragColor = vec4(0.0, rainbow_G(time), 0.0, 1.0);
	//FragColor = vec4(0.0, 0.0, rainbow_B(time), 1.0);
}

float rainbow_R(float seconds)
{
	// Piecewise function for the R value

	// Y (Red) value starts at 1
	// Value = 1
	if ((n <= seconds && seconds <= n+1) || (n+5 <= seconds && seconds <= n+6))
	{
		return 1;
	}
	// Slope down
	else if (n+1< seconds && seconds < n+2)
	{
		return 0.5+0.5*cos(seconds*PI - PI);
	}
	// Value = 0
	else if (n+2 <= seconds && seconds <= n+4)
	{
		return 0;
	}
	// Slope up
	else if (n+4 < seconds && seconds < n+5)
	{
		return 0.5+0.5*cos(seconds*PI - PI);
	}
}

float rainbow_G(float time)
{
	// Piecewise function for the G value

	// Y (Green) value starts at 0
	// Slope up
	if (n < time && time < n+1)
	{
		return 0.5+0.5*cos(time*PI - PI);
	}
	// Value = 1
	else if (n+1 <= time && time <= n+3)
	{
		return 1;
	}
	// Slope down
	else if (n+3 < time && time < n+4)
	{
		return 0.5+0.5*cos(time*PI - PI);
	}
	// Value = 0
	else if (n+4 <= time && time <= n+6)
	{
		return 0;
	}
}

float rainbow_B(float time)
{
	// Piecewise function for the B value

	// Y (Blue) value starts at 0
	// Value = 0
	if (n <= time && time <= n+2)
	{
		return 0;
	}
	// Slope up
	else if (n+2 < time && time < n+3)
	{
		return 0.5+0.5*cos(time*PI - PI);
	}
	// Value = 1
	else if (n+3 <= time && time <= n+5)
	{
		return 1;
	}
	// Slope down
	else if (n+5 < time && time < n+6)
	{
		return 0.5+0.5*cos(time*PI - PI);
	}
}
