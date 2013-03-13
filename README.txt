TARDIS
======

The TARDIS module allows you to organise your recent nodes chronologically 
using a block which displays links in the format "YYYY/MM". There's several 
options available, so install as usual(*) and visit 
example.com/structure/tardis to config both the TARDIS block and the TARDIS 
page. 

(*) Download the module from http://Drupal.org/node/1939164 and extract in 
your drupal installation under sites/all/modules. 

Some notes on the more obscure settings: 


Tardis block settings
---------------------

- "How far back?" means how many links will be displayed in the TARDIS block. 
So if you put 36 months, it will attempt to retrieve 36 months' worth of posts 
(months in which there is no content are not counted.)

- "Stop looking at the year" means the TARDIS won't look beyond a certain year 
in the past. 
As far as I'm concerned, there's no reasonable way to know how long have people 
been posting, unless you look into the entire node table. That's quite a lot of 
queries, so I thought I could put a year beyond which you're sure it's 
pointless to look. 

- "Custom link" means you don't have to use the TARDIS page if you have 
something better - for example, a view. The TARDIS page is pretty simple: a 
bunch of node teasers, filtered by year (YYYY) and month (MM) as separate 
arguments, and that's it. So if you have a view, say, 
example.com/recent/nodes/view, put "recent/nodes/view" here and the TARDIS 
will take care of the rest. 


Tardis page settings
--------------------

Everything here is pretty self-explanatory, so play with the settings until 
they suit your needs. Please note: if you change either the page title or 
address, you'll have to rebuild the cache. The TARDIS will display a handy 
link where you can do just that. 


Special thanks
==============

- My mom (hi mom!) 
- Prometheus6 for the original idea: http://drupal.org/project/montharchive
- Drupalize.me for their awesome videos and endless patience. If you wanna 
build your module, and you wanna build it right, go there. 
