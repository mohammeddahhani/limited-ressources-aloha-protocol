# Introduction:

This repository contains an algorithm to compute the expected number of successful transmissions in a limited-ressource 
Aloha channel with a finite number of contending stations

# Problem Definition:

$N$ stations are trying to transmit a packet over one of the $N_s$ available time slots.
Since all stations are contending over the same number of ressource, a collision may occur between two or multiple transmitting stations in which case their retransmit again.

At the begining, all stations start by uniformly choosing a random backoff in $[0,N_s-1]$.
When multiple stations choose the same backoff value they collide and their transmission fails. As a consesequence each colliding station performs another backoff by uniformly selecting a number in $[0,N_s-1]$ and wait a corresponding number of time slots before trying again.

This process repeats until all collisions are resolved in which case we want to know how many stations succeeded their transmissions. That is, we are want to find the probability distribution of the number of successfull transmissions once all collisions have been resolved.

# Model Description 

Let $n=(n_1,n_2,...,n_{N_s})$ be a possible initial state of the system where $n_i$ is the number of stations that chose slot $i$. We have  $\sum_{i=1}^{N} n_i } = N$ . The probability of a possible initial state is:

$$
P\{ n=(n_1,n_2,...,n_{N_{s}}) \}=
\frac{
{N \choose n_1} \, {N-n_1 \choose n_2} \, {N-n_1-n_2 \choose n_3} \, ... \,
{N-n_1-n_2- ... -n_{N_{s}-2}\choose n_{N_{s}-1}}
}
{N_{s}^N} 

$$

A station that collides and choose a backoff greater or equal than the number of remaining slots will quite the race and won't be able to succeed its transmission.
In this case, we can imagine that such a station choses a imaginary time slot beyond slot $N_s$ in which all transmission are destined to fail. 

<div style="text-align:center"><img src="https://github.com/mohammeddahhani/limited-ressources-Aloha-protocol/blob/master/exact_model.png" width="500" height="250"/></div>

Now let $n=(n_1,n_2,...,n_{N_s},n_{\phi})$ be any possible state of the systems such that  $\sum_{i=1}^{N} n_i } + n_{\phi} = N$, where $n_{\phi}$ is the number of stations that chose the imaginary slot.
As the above figure shows, if $n_i > 1$ the $n_i$ collding stations will uniformly be distributed over the $N-i+1$ remaining slots.


# Algorithmic Solution:

Our algorithm iteratively runs the process described in the figure above for a given number of stations $N$ and time slots $N_s$ until all collisions are resolved. We use it to compute the expected number of having $k$ success given $N$ and $N_s$.

# Requirements:

A working installation of the GNU C++ Compiler is required.

Tested with gcc version 5.4.0. 

# Usage:
`./compile.sh`

`./success_rate n ns` where `n` is the number of contending stations and `ns` the number of time slots
