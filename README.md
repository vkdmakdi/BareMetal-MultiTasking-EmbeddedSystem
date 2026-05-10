Bare-Metal Architecture: Bypassed the standard HAL/LL libraries to interact directly with the MCU’s memory-mapped registers. This involved manual configuration of the RCC (Reset and Clock Control) to manage power and timing for peripherals.

Peripheral Mastery:
GPIO: Configured pins for multiple modes (Input, Output, and Alternate Function).
Timers (TIM2): Learned to configure hardware timers for PWM (Pulse Width Modulation) to control LED brightness without CPU intervention.
Interrupts (NVIC & EXTI): Implemented an event-driven model where physical button presses trigger asynchronous code execution via the Nested Vectored Interrupt Controller.
Custom Timing Engine: Built a non-blocking timing system by tapping into the SysTick (the ARM core's internal 24-bit counter). This allowed to create a millis()-style function, enabling the CPU to multitask rather than being stuck in "busy-wait" delays.
State Machine Logic: You developed an asynchronous app_task that manages an LED state based on global flags set by hardware interrupts, demonstrating a professional approach to firmware structure.

The Stack
Hardware: Nucleo-G431RB (STM32G431 MCU).
Development Environment: STM32CubeIDE (used as a compiler/debugger only, skipping the code generator).
Reference Material: Direct utilization of the STM32G4 Datasheet and Reference Manual for register offsets and bit definitions.
