#ifndef header_h
#define header_h

String file1=
"<!doctype html>\r\n"
"<html>\r\n"
"<meta http-equiv=cache-control content=\"no-cache\"/>\r\n"
"<meta http-equiv=pragma content=no-cache />\r\n"
"<script>window.onload=function(){var S=1;var Q=0;var G=0;var r=0;var m=0;var A=true;var k=true;var I=true;var w=0;var B=0;var d=0;var R=0;var M=0;var z=0;var y=0;var L=0;D();var v=setInterval(D,1000*30);function q(T){return document.getElementById(T)}function J(T){return document.getElementsByClassName(T)[0]}function s(T){return document.getElementsByTagName(T)[0]}g();n(\"hours_on\");n(\"hours_off\");n(\"sleep_h\");h(\"sleep_m\");h(\"minutes_on\");h(\"minutes_off\");q(\"sleep_h\").addEventListener(\"change\",function(){w=q(\"sleep_h\").selectedIndex;F()});q(\"sleep_m\").addEventListener(\"change\",function(){B=q(\"sleep_m\").selectedIndex;F()});q(\"sleep\").addEventListener(\"click\",function(){console.log(\"click sleep!\");if(k){F()}else{H()}});q(\"auto\").addEventListener(\"click\",function(){if(A){f()}else{x()}});q(\"manual\").addEventListener(\"click\",function(){if(I){j()}else{E()}});q(\"on\").addEventListener(\"click\",function(){l(\"&0&1&\")});q(\"off\").addEventListener(\"click\",function(){l(\"&0&0&\")});q(\"loader_m\").addEventListener(\"click\",function(){D()});q(\"loader_s\").addEventListener(\"click\",function(){D()});q(\"led\").addEventListener(\"click\",function(){D()});q(\"sleep_send\").addEventListener(\"click\",function(){C()});J(\"send\").addEventListener(\"click\",function(){var W=q(\"mode\").selectedIndex;if(W==3){return}var T=parseInt(J(\"number\").innerHTML);console.log(\"Program mode:\"+W);switch(W){case 0:p(T);break;case 1:Q=q(\"hours_on\").selectedIndex;G=q(\"minutes_on\").selectedIndex;r=q(\"hours_off\").selectedIndex;m=q(\"minutes_off\").selectedIndex;console.log(Q);console.log(G);console.log(r);console.log(m);var U=Q*60+G;var V=r*60+m;if(isNaN(T)){T=1}b(T,1,U,V);break}});J(\"left\").addEventListener(\"click\",function(){console.log(\"Click Left arrow\");S--;if(S<1){S=10}e(\"/status?&2&\"+S+\"&\")});J(\"right\").addEventListener(\"click\",function(){console.log(\"Click Right arrow\");S++;if(S>10){S=1}e(\"/status?&2&\"+S+\"&\")});q(\"mode\").addEventListener(\"change\",function(){Q=0;q(\"hours_on\").selectedIndex=Q;G=0;q(\"minutes_on\").selectedIndex=G;r=0;q(\"hours_off\").selectedIndex=r;m=0;q(\"minutes_off\").selectedIndex=m;g()});function p(T){l(\"&2&\"+T+\"&0&\")}function b(T,W,U,V){l(\"&2&\"+T+\"&\"+W+\"&\"+U+\"&\"+V+\"&\")}function n(U){for(i=0;i<24;i++){var T=document.createElement(\"option\");if(i<10){T.text=\"0\"+i}else{T.text=i}q(U).add(T)}}function h(U){for(i=0;i<60;i++){var T=document.createElement(\"option\");if(i<10){T.text=\"0\"+i}else{T.text=i}q(U).add(T)}}function f(){J(\"programSetup\").style.display=\"block\";A=false;if(!k){H()}if(!I){E()}}function x(){J(\"programSetup\").style.display=\"none\";A=true}function F(){var U=new Date();var T=U.getHours();var V=U.getMinutes();J(\"sleep\").style.display=\"block\";x();E();k=false}function H(){J(\"sleep\").style.display=\"none\";k=true}function j(){if(!A){x()}if(!k){H()}J(\"manual\").style.display=\"block\";I=false}function E(){J(\"manual\").style.display=\"none\";I=true}function C(){d=w*60+B;l(\"&1&\"+d+\"&\")}function a(){J(\"number\").innerHTML=S;console.log(y);q(\"mode\").selectedIndex=y;console.log(Q+\":\"+G+\";\"+r+\":\"+m);q(\"hours_on\").selectedIndex=Q;q(\"minutes_on\").selectedIndex=G;q(\"hours_off\").selectedIndex=r;q(\"minutes_off\").selectedIndex=m;g()}function g(){var T=q(\"mode\").selectedIndex;J(\"left\").style.visibility=\"visible\";J(\"right\").style.visibility=\"visible\";J(\"number\").style.visibility=\"visible\";J(\"sleep\").style.display=\"none\";J(\"ON\").style.display=\"none\";J(\"OFF\").style.display=\"none\";q(\"data\").style.display=\"none\";switch(T){case 0:break;case 1:J(\"ON\").style.display=\"block\";J(\"OFF\").style.display=\"block\";q(\"selectors\").style.margin=\"10px auto\";break;case 2:q(\"data\").style.display=\"block\";J(\"ON\").style.display=\"block\";J(\"OFF\").style.display=\"block\";q(\"selectors\").style.margin=\"0px auto\";break}}function u(T){T=\"1&2&3\"+T;T=T.split(\"&\");N(T)}function K(V){V=V.split(\"&\");var T=V.length;console.log(\"Len of data is:\"+T);if(T<2){t(V);return}var W=V[1];var U=V[2];O(U);if(W==0){console.log(\"Manual Mode\");c()}else{if(W==1){console.log(\"Sleep Mode\");if(T>4){d=V[3];console.log(\"sleep:\"+d);o(d)}return}else{if(W==2){console.log(\"Auto Mode\");if(T>4){P(V)}else{t(V);return}}}}}function O(T){console.log(\"Power status:\"+T);if(T==1){console.log(\"Power status:\"+T);q(\"led\").style.backgroundColor=\"Chartreuse\"}if(T==0){console.log(\"Power status:\"+T);q(\"led\").style.backgroundColor=\"Crimson\"}}function o(T){L=0;w=parseInt(T/60);B=T-w*60;q(\"sleep_m\").selectedIndex=B;q(\"sleep_h\").selectedIndex=w;q(\"sleep\").style.backgroundColor=\"green\";q(\"manual\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"auto\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";F()}function c(T){q(\"manual\").style.backgroundColor=\"green\";q(\"sleep\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"auto\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";d=0;L=0;q(\"sleep_m\").selectedIndex=0;q(\"sleep_h\").selectedIndex=0}function P(T){q(\"auto\").style.backgroundColor=\"green\";q(\"manual\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"sleep\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";N(T)}function N(T){S=T[3];y=T[4];Q=parseInt(T[5]/60);G=T[5]-Q*60;r=parseInt(T[6]/60);m=T[6]-r*60;L=T[7];f();a()}function t(T){console.log(\"Error in status data:\"+T)}function D(){console.log(\"Read status..\");if(L!=0){e(\"/status?&2&\"+L+\"&\")}e(\"/status?\")}function l(T){console.log(\"Send command\"+T);e(\"/comm\"+T)}function e(T){q(\"loader_m\").innerHTML=\"Loading...\";q(\"loader_s\").innerHTML=\"Loading...\";q(\"loader_a\").innerHTML=\"Loading....\";var U=new XMLHttpRequest();U.onreadystatechange=function(V){if(U.readyState===4&&U.status===200){if(U.responseText.search(\"status&\")==0){q(\"loader_m\").innerHTML=\"Load Ok!\";q(\"loader_s\").innerHTML=\"Load Ok!\";q(\"loader_a\").innerHTML=\"Load Ok!\";K(U.responseText)}else{if(U.responseText.search(\"program&\")==0){q(\"loader_a\").innerHTML=\"Load Ok!\";u(U.responseText)}}}else{if(U.readyState===4&&U.status===0){q(\"led\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"manual\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"sleep\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"auto\").style.backgroundColor=\"rgba(128, 128, 128, 0.48)\";q(\"loader_m\").innerHTML=\"Load Error!\";q(\"loader_s\").innerHTML=\"Load Error!\";q(\"loader_a\").innerHTML=\"Load Error!\"}}};U.open(\"POST\",T+Math.random(),true);U.send()}};</script>\r\n"
"<style>div{-webkit-touch-callout:none;-webkit-user-select:none;-khtml-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none;outline:0}body{text-align:center;padding:0;margin:0}h1,h2{display:inline-block;margin:10px 0 10px 0;text-align:center;border-radius:8px;cursor:pointer;background-color:rgba(128,128,128,0.48);width:300px;user-select:none}div.programSetup{display:none}label{text-align:center}div#ON{font-size:24px;background-color:gray;width:150px;margin:0 auto -6px auto;border-radius:8px}select{-webkit-appearance:none;-moz-appearance:none;appearance:none;border-style:none;cursor:pointer;font-size:32px;outline:0;background-color:rgba(158,158,158,0)}select::-ms-expand{display:none}div#program{display:inline-block;padding-top:5px;background-color:rgba(158,158,158,0.56);height:210px;width:170px;margin:0 auto;border-radius:8px}ul{list-style:none;text-align:center}li{display:inline}.number{font-size:32px;margin:10px 0 0 0}div.left{float:left;cursor:pointer;font-size:72px;margin:60px 0 0 0}div.right{float:right;cursor:pointer;font-size:72px;margin:60px 0 0 0}div.left:active{transform:translateX(-4px)}div.right:active{transform:translateX(+4px)}button.left{float:left;cursor:pointer;font-size:48px;margin:0}button.right{float:right}.send{cursor:pointer;margin:15px 0 5px 0;background-color:gray;border-radius:8px;height:40px;font-size:36px}.send:active{transform:translateY(-4px)}#data{width:150px;border-radius:8px;margin:0 auto 9px auto;display:none}#data:focus #mode:focus{outline:0}.right-arrow:after{font-size:42px;content:'\\25BA'}.left-arrow:before{font-size:42px;content:'\\25C4';clear:both}div#selectors{width:100px;height:50px;margin:0 auto;border-radius:8px;display:none}div.status{margin:18px auto 0;cursor:pointer}div#selectors.sleep{margin:28px auto;display:'none'}div.programSetup h3{display:inline-block;margin:10px 0 10px 0;text-align:center;border-radius:8px;background-color:rgba(128,128,128,0.48);width:200px;user-select:none}div.manual{display:none}div.button{cursor:pointer;padding:20px;margin:10px 0 0 0;background-color:gray;border-radius:8px;height:50px;font-size:36px}#off.button{margin:30px 0 0 0}div.button:active{transform:translateY(-4px)}</style>\r\n"
"<body>\r\n"
"<h1 id='timer setup'>TIMER SETUP</h1>\r\n"
"<br>\r\n"
"<h1 id=led>LED</h1>\r\n"
"<br>\r\n"
"<h2 id=manual> MANUAL </h2>\r\n"
"<div class=manual style='width:200px;margin:0 auto'>\r\n"
"<div id=on class=button>ON</div>\r\n"
"<div id=off class=button>OFF</div>\r\n"
"<div id=loader_m style=\"margin:10px auto;cursor:pointer\">Loading...</div>\r\n"
"</div>\r\n"
"<br>\r\n"
"<h2 id=auto> AUTO </h2>\r\n"
"<br>\r\n"
"<div class=programSetup>\r\n"
"<h3> PROGRAM SETUP </h3>\r\n"
"<div style='text-align:center;margin:0 auto;width:300px'>\r\n"
"<div class=left>&#171;</div>\r\n"
"<div id=program>\r\n"
"<select id=mode>\r\n"
"<option val=0>None</option>\r\n"
"<option val=1>Daily</option>\r\n"
"</select>\r\n"
"<input id=data type=date></input>\r\n"
"<div id=selectors class=ON>\r\n"
"<span style=display:inline-block;margin:2px>ON</span>\r\n"
"<br>\r\n"
"<select id=hours_on></select><span style=font-size:24px;vertical-align:text-bottom>:</span><select id=minutes_on></select>\r\n"
"</div>\r\n"
"<br>\r\n"
"<div id=selectors class=OFF>\r\n"
"<span style=display:inline-block;margin:2px>OFF</span>\r\n"
"<br>\r\n"
"<select id=hours_off></select><span style=font-size:24px;vertical-align:text-bottom>:</span><select id=minutes_off></select>\r\n"
"</div>\r\n"
"</div>\r\n"
"<div class=right>&#187;</div>\r\n"
"</div>\r\n"
"<div style='width:200px;margin:0 auto'>\r\n"
"<div class=number></div>\r\n"
"<div class=send>Send</div>\r\n"
"<div id=loader_a>Loading...</div>\r\n"
"</div>\r\n"
"</div>\r\n"
"<h2 id=sleep> SLEEP </h2>\r\n"
"<div class=sleep id=selectors style='margin:0 auto;font-size:24px'>\r\n"
"<span>Off after</span>\r\n"
"<br>\r\n"
"<select id=sleep_h></select><span style=font-size:32px;vertical-align:text-bottom>:</span><select id=sleep_m></select>\r\n"
"<br>\r\n"
"<div id=sleep_send class=send>Send</div>\r\n"
"<div id=loader_s>Loading...</div>\r\n"
"</div>\r\n"
"</body>\r\n"
"</html>\r\n";

#endif