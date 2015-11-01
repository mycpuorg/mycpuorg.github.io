---
layout: post
title: "Options: Elementary Strategies"
excerpt: "Options Elementary Strategies"
tags: [Investing, Strategies, Options, Calls, Puts 
comments: true
---

Options: Elementary Strategies - Part I

A new trader will be faced with a deluge of numbers on the options charts listing options pricing.
An equity is identified by a unique symbol, the last price that the transaction was made at.

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">


<colgroup>
<col  class="left" />

<col  class="right" />

<col  class="right" />

<col  class="right" />
</colgroup>
<thead>
<tr>
<th scope="col" class="left">Symbol</th>
<th scope="col" class="right">Last</th>
<th scope="col" class="right">Bid</th>
<th scope="col" class="right">Ask</th>
</tr>
</thead>

<tbody>
<tr>
<td class="left">XYZ</td>
<td class="right">99.06</td>
<td class="right">98.89</td>
<td class="right">99.21</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">&#xa0;</td>
<td class="right">&#xa0;</td>
<td class="right">&#xa0;</td>
</tr>
</tbody>
</table>

For options pricing the chart looks drastically different. In it's simplest form this is what a typical chart looks like for options.
XYZ: Options pricing chart for single expiry date:

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">


<colgroup>
<col  class="left" />

<col  class="right" />

<col  class="right" />

<col  class="left" />

<col  class="right" />

<col  class="left" />

<col  class="right" />

<col  class="right" />

<col  class="left" />
</colgroup>
<thead>
<tr>
<th scope="col" class="left">Calls&#x2013;></th>
<th scope="col" class="right">Bid</th>
<th scope="col" class="right">Ask</th>
<th scope="col" class="left"><&#x2013;Calls</th>
<th scope="col" class="right">Strike</th>
<th scope="col" class="left">Puts&#x2013;></th>
<th scope="col" class="right">Bid</th>
<th scope="col" class="right">Ask</th>
<th scope="col" class="left"><&#x2013;Puts</th>
</tr>
</thead>

<tbody>
<tr>
<td class="left">&#xa0;</td>
<td class="right">13.9</td>
<td class="right">14.1</td>
<td class="left">&#xa0;</td>
<td class="right">85</td>
<td class="left">&#xa0;</td>
<td class="right">0.1</td>
<td class="right">0.15</td>
<td class="left">&#xa0;</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">8.9</td>
<td class="right">9.1</td>
<td class="left">&#xa0;</td>
<td class="right">90</td>
<td class="left">&#xa0;</td>
<td class="right">0.36</td>
<td class="right">0.39</td>
<td class="left">&#xa0;</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">3.9</td>
<td class="right">4.1</td>
<td class="left">&#xa0;</td>
<td class="right">95</td>
<td class="left">&#xa0;</td>
<td class="right">1.42</td>
<td class="right">1.45</td>
<td class="left">&#xa0;</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">2.3</td>
<td class="right">2.5</td>
<td class="left">&#xa0;</td>
<td class="right">100</td>
<td class="left">&#xa0;</td>
<td class="right">2.4</td>
<td class="right">2.6</td>
<td class="left">&#xa0;</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">1.4</td>
<td class="right">1.6</td>
<td class="left">&#xa0;</td>
<td class="right">105</td>
<td class="left">&#xa0;</td>
<td class="right">5.21</td>
<td class="right">5.95</td>
<td class="left">&#xa0;</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">0.45</td>
<td class="right">0.49</td>
<td class="left">&#xa0;</td>
<td class="right">110</td>
<td class="left">&#xa0;</td>
<td class="right">10.8</td>
<td class="right">11.2</td>
<td class="left">&#xa0;</td>
</tr>


<tr>
<td class="left">&#xa0;</td>
<td class="right">0.2</td>
<td class="right">0.23</td>
<td class="left">&#xa0;</td>
<td class="right">115</td>
<td class="left">&#xa0;</td>
<td class="right">15.7</td>
<td class="right">16.34</td>
<td class="left">&#xa0;</td>
</tr>
</tbody>
</table>

We will try and break it down to simpler sub-parts:

-   The Calls and Puts table are listed for strike prices near
    the current trading price for XYZ
-   Remember that the table above shows options pricing only for a
    single expiry date.
-   The table can be split into two major parts: for Calls and Puts
    showing the Bid and Ask prices.
-   The table also shows the volatility skew in pricing between Bid
    and Ask prices towards the ends of the table.

As we see from the equity pricing that XYZ is trading at around 99 mark currently.

Based on the current equity price the options pricing of the various strikes varies constantly determined by various factors which are covered in the writeup about the 'Greeks'.

For the sake of simplicity, our discussion can be limited to the pricing listed on this table while ignoring the effect of Greeks and volatility.

## Calls:

Suppose we believe that the price of underlying equity is going to increase all the way up to 108 before expiration, then we might purchase a 100 Call (Call at strike 100) for 2.5

-   If we are correct and the price of the option goes beyond 108 prior
    to expiration then we have the following two choices:
    i) purchase 100 stocks of XYZ from the contract writer at 108 each
    and sell them in the open market for a profit (determined by the
    current price of XYZ). A detail here is the price paid for the
    purchase of the call option 2.5 (2.5 \* 100 = 250 for each 100
    Call). In effect the price of XYZ should rise above 110.5 for us
    to start making profit with route.
    ii) sell the Call option in the open market whose value would be
    higher than 2.5 for a profit (2.5 is a rough estimate as the price of the
    option whose strike is at current price of XYZ). The detail here
    is that the option price should rise to 108 with sufficient time
    to expiration remaining. This is due to a positive intrinsic value
    of the options that can be attributed to the time remaining to
    expiration and the proximity of the current price to strike price
    of the option in question.

Both of the above choices should result in a profit if there is sufficient time before the option expiration because the intrinsic value of the call option becomes higher than the current price at expiration.

## Puts:

Suppose we believe that the price of underlying equity is going to decrease all the way down to 82 before expiration, then we might purchase a 90 Puts (Puts at strike 90) for 0.39

-   If we are correct and the price of the option goes below 90 prior
    to expiration then we have the following two choices:
    i) Purchase 100 stocks of XYZ from open market at the current
    price (which is < 90) 
    and sell them to the contract writer at 90 for a profit (determined by the
    current price of XYZ). A detail here is the price paid for the
    purchase of the puts option 0.39 (0.39 \* 100 = 39 for each 100
    Puts). In effect the price of XYZ should fall below 89.61 for us
    to start making profit with route.
    ii) sell the Puts option in the open market whose value would be
    higher than 2.4 for a profit (2.4 is a rough estimate as the price of the
    option whose strike is at current price of XYZ). The detail here
    is that the option price should fall below 90 with sufficient time
    to expiration remaining. This is due to a positive intrinsic value
    of the options that can be attributed to the time remaining to
    expiration and the proximity of the current price to strike price
    of the option in question.

Both of the above choices should result in a profit if there is sufficient time before the option expiration because the intrinsic value of the puts option becomes higher than the current price at expiration.
