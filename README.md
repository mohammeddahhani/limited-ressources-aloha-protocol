# Introduction:

This repository contains an algorithm to compute the expected number of successful transmissions in a limited-ressource 
Aloha channel with a finite number of contending stations

# Problem definition:

<img src="/tex/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode&sanitize=true" align=middle width=14.99998994999999pt height=22.465723500000017pt/> stations are trying to transmit a packet over one of the <img src="/tex/29fb78801f1c792da0c00b3ed4246275.svg?invert_in_darkmode&sanitize=true" align=middle width=19.41213779999999pt height=22.465723500000017pt/> available time slots.
Since all stations are contending over the same number of ressource, a collision may occur between two or multiple transmitting stations in which case their retransmit again.

At the begining, all stations start by uniformly choosing a random backoff in <img src="/tex/14a475464f068c9a3f7677679a2385db.svg?invert_in_darkmode&sanitize=true" align=middle width=73.2019926pt height=24.65753399999998pt/>.
When multiple stations choose the same backoff value they collide and their transmission fails. As a consesequence each colliding station performs another backoff by uniformly selecting a number in <img src="/tex/14a475464f068c9a3f7677679a2385db.svg?invert_in_darkmode&sanitize=true" align=middle width=73.2019926pt height=24.65753399999998pt/> and wait a corresponding number of time slots before trying again.

We aim to find the probability distribution of the number of success transmissions once all collisions have been resolved.

# Model description 

Let <img src="/tex/59f3a42931cc7c8e988fc88011c1f47d.svg?invert_in_darkmode&sanitize=true" align=middle width=142.09983974999997pt height=24.65753399999998pt/> be a possible initial state of the system where <img src="/tex/de3e4ddbaf93c2db6b330ad1998cc995.svg?invert_in_darkmode&sanitize=true" align=middle width=14.517775799999992pt height=14.15524440000002pt/> is the number of stations that chose slot <img src="/tex/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663225699999989pt height=21.68300969999999pt/>. We have  <img src="/tex/713e13330ab3e1f1d5123c74af238ace.svg?invert_in_darkmode&sanitize=true" align=middle width=94.46531159999999pt height=32.256008400000006pt/> . The probability of a possible initial state is:

<p align="center"><img src="/tex/c6bd088e95ff412f19d37bdbf7252e72.svg?invert_in_darkmode&sanitize=true" align=middle width=524.63264205pt height=45.87321255pt/></p>

A station that collides and choose a backoff greater or equal than the number of remaining slots will quite the race and won't be able to succeed its transmission.
In this case, we can imagine that such a station choses a imaginary time slot beyond slot <img src="/tex/29fb78801f1c792da0c00b3ed4246275.svg?invert_in_darkmode&sanitize=true" align=middle width=19.41213779999999pt height=22.465723500000017pt/> in which all transmission are destined to fail. 

<div style="text-align:center"><img src="https://github.com/mohammeddahhani/limited-ressources-Aloha-protocol/blob/master/exact_model.png" width="500" height="250"/></div>

Now let <img src="/tex/f7e9bfdb04bf00e6004d08e00908cca8.svg?invert_in_darkmode&sanitize=true" align=middle width=167.99862749999997pt height=24.65753399999998pt/> be any possible state of the systems such that  <img src="/tex/1c61197b8ba62acc435a1cc843958e14.svg?invert_in_darkmode&sanitize=true" align=middle width=133.14940814999997pt height=32.256008400000006pt/>, where <img src="/tex/9f42f25156036208d2dc90066e5133f9.svg?invert_in_darkmode&sanitize=true" align=middle width=17.771009849999988pt height=14.15524440000002pt/> is the number of stations that chose the imaginary slot.
As the follwing figure shows, if <img src="/tex/ddc076dcde48b1b39299b8bf536b0b73.svg?invert_in_darkmode&sanitize=true" align=middle width=45.476512949999986pt height=21.18721440000001pt/> the <img src="/tex/de3e4ddbaf93c2db6b330ad1998cc995.svg?invert_in_darkmode&sanitize=true" align=middle width=14.517775799999992pt height=14.15524440000002pt/> stations will uniformly be distributed over the <img src="/tex/93135102bf51009e249a0ff86ed8681e.svg?invert_in_darkmode&sanitize=true" align=middle width=69.06478754999999pt height=22.465723500000017pt/> remaining slots:


# Algorithmic solution:

Our algorithm iteratively runs the process described in the figure above for a given number of stations <img src="/tex/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode&sanitize=true" align=middle width=14.99998994999999pt height=22.465723500000017pt/> and time slots <img src="/tex/29fb78801f1c792da0c00b3ed4246275.svg?invert_in_darkmode&sanitize=true" align=middle width=19.41213779999999pt height=22.465723500000017pt/> until all collisions are resolved. We use it to compute the probability of having <img src="/tex/63bb9849783d01d91403bc9a5fea12a2.svg?invert_in_darkmode&sanitize=true" align=middle width=9.075367949999992pt height=22.831056599999986pt/> success given <img src="/tex/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode&sanitize=true" align=middle width=14.99998994999999pt height=22.465723500000017pt/> and <img src="/tex/29fb78801f1c792da0c00b3ed4246275.svg?invert_in_darkmode&sanitize=true" align=middle width=19.41213779999999pt height=22.465723500000017pt/>.

# Requirements:

A working installation the GNU C++ Compiler is required.

Tested with gcc version 5.4.0. 

# Usage:
`./compile.sh`

`./success_rate n ns` where `n` is the number of contending stations and `ns` the number of time slots
