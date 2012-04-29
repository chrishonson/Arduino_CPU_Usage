Arduino_CPU_Usage
=================

Understanding processor load in an embedded system is important, yet often overlooked. It’s a step toward analysing your processor’s ability to meet system deadlines. I have provided a sample arduino sketch to show how you can add real-time CPU utilization measurements to your project. 

CPU utilization is simply the ratio of time a processor spends doing real work over a given period of time. The time of measurement can be arbitrary, or it can depend on the shortest deadline in your project. It depends on the goals of the CPU utilization measurement. If you have to service a communication bus 10ms after receiving an input, and it takes 9ms to perform the necessary processing, then you may want to measure utilization over 10ms so that you can capture this 90% worst case, critical path. For the purpose of simplicity and illustration I will measure over 1s. 

The first thing we are going to need is a reliable timing source. I recommend setting up a timer ISR. I could explain how to, but luckily someone else already has here. In my example I chose to use timer 2 since timer 1 is used by the servo library and I might actually use that in the future. 

The next step in setting up your project is defining what 0% utilization is. The most basic way of doing this is by incrementing a counter in your idle task (in this case loop()) and seeing how many idle counts occur during a measurement period. If no work is being done (besides the timer isr) then this represents the maximum number of idle counts and 0% utilization. One might argue that the act of calculating idle counts is work and that 0% utilization is not achievable with the instrumentation code in place. That’s true, but I think such concerns are negligible when the CPU utilization measurement period is sufficiently large.

Anyway, it’s important to note, here, that once you determine the maximum idle counts, no code can be added to the idle task. This would change the maximum idle counts. In general, I think the idle task should do essentially nothing. Where then can you get useful work done?

My example provides a very simple, round robin, periodic task scheduler. Here you can perform calculations as fast or as slow as required. It’s also noteworthy that I used the ISR only to notify tasks that they are ready to run. It’s good practice to keep ISR’s as fast as possible and to keep application code out in order to prevent problems with shared data. 
 
To test my example I simply add a delay(50) to my 100ms task. This will run 10 times in one second. 10*50=500ms I should calculate 50% utilization and if I run the example I see that it does.