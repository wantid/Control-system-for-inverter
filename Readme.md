# Control system forming PWM

The control system generates control impulses for invertor.

## Description

This repository contains a programm wrote on *C* packed in Keil uVision 5 project. The program code works on the basis of STM32 «F767ZI» debug board.

## Development

In the inverter under consideration, the control signals of the converter keys are formed by comparing two analog signals. These signals are fed to different inputs of the comparators, and in the case when the signal at the non-inverting input of the comparator is greater in value than at the inverting one, a logical “1” is formed at the output of the comparator, otherwise “0”. Timers were used to generate control signals using a microcontroller based on a debug board.

<p align="center"><img width="50%" src="/GithubMedia/timers.png" alt="timers description"></p>
<p align="center">fig.1 - timers' description</p>

Each clock pulse arriving at the counting input changes the value in the CNT counting register by one. In bi-directional counting mode, the timer counts up first from 0 to the limit value, then down to 0. The count limit is determined by the value written to the ARR register. The value itself is calculated using the following formula:

<p align="center"><img src="/GithubMedia/arr.PNG" alt="ARR value"></p>
<p align="center">fig.2 - Value for auto reload register</p>

If the value in the counting register CNT coincides with the value written in the capture / compare register (CCR) of any timer channel, the logical states of the outputs of this channel are switched, which is similar to the formation of a PWM signal using a comparator. On the counting period of the timer, there are two such switchings on each channel. The values in the CCR register change from period to period of the count, reproducing in the form of a code, in fact, a sinusoidal reference signal. The recalculation of sinusoidal signals and updating the values for the CCR registers can be done in an interrupt routine.

<p align="center"><img src="/GithubMedia/sinus.png" alt="Sin formulas"></p>
<p align="center">fig.3 - Sin formulas</p>

The generated program code was uploaded to the debug board in order to test the correct operation of the generated signals. The figure shows oscillograms of two complementary signals:

<p align="center"><img width="50%" src="/GithubMedia/compSignals.png" alt="Complementary signals"></p>
<p align="center">fig.4 - Complementary signals</p>

### Speed feedback formation

The speed sensor used in the tests sends 60 pulses per 1 full revolution. At a nominal rotation speed of 1000 rpm, the sensor will send 60000 pulses/min. It was decided to evaluate the number of pulses coming from the sensor in 1 second. Thus, at a speed of 1000 rpm, we get:

<p align="center"><img src="/GithubMedia/1000pulses.PNG" alt="Pulses formula"></p>
<p align="center">fig.5 - Pulses formula</p>

Two timers were used to implement pulse counting. TIM6 is used for timing in the form of 1 second. TIM2 is used to count pulses for the specified time. At the end of the TIM6 count, the resulting value of the pulses is converted into a frequency, after which the points parameter is overwritten and the subsequent recalculation of the sin formulas. Testing and debugging of this algorithm was carried out using the debug mode in the Keil uVision 5 environment, together with tracking real indicators using the MAGTROL DSP6001 dynamometer controller.

<p align="center"><img width="50%" src="/GithubMedia/counting.png" alt="Pulses counting"></p>
<p align="center">fig.5 - Pulses counting in debug mode (Keil uVision 5)</p>

## Links

[Telegram] [Youtube]

[Youtube]: https://www.youtube.com/channel/UC3kV-wnqBE3Y2tdtdSrjvGQ
[Telegram]: https://t.me/exeersitus

