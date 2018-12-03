"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<meta http-equiv=\"cache-control\" content=\"no-cache\"/>\r\n"
"<meta http-equiv=\"pragma\" content=\"no-cache\" />\r\n"
"<script>\r\n"
"    window.onload=function(){	\r\n"
"        var number=1;\r\n"
"        var hour_on = 0;\r\n"
"        var minute_on=0;\r\n"
"        var hour_off= 0;\r\n"
"        var minute_off=0;\r\n"
"        var displayAuto=true;\r\n"
"        var displaySleep=true;\r\n"
"        var displayManu=true;\r\n"
"        var sleepHour=0;\r\n"
"        var sleepMinute=0;\r\n"
"        var sleep=0;\r\n"
"        var power=0;\r\n"
"        var hourOff=0;\r\n"
"        var minuteOff=0;\r\n"
"        var programMode=0;\r\n"
"        var programPowerNumber=0;      \r\n"
"        var check=setInterval(readStatus, 1000*30);\r\n"
"        var urladdress=window.location;\r\n"
"        function byId(id){return document.getElementById(id)};\r\n"
"        function byClass(id){return document.getElementsByClassName(id)[0]};\r\n"
"        function byTag(id){return document.getElementsByTagName(id)[0]};	\r\n"
"		readStatus();\r\n"
"		 //ShowNumber();\r\n"
"        Status();\r\n"
"        selectHour('hours_on');\r\n"
"        selectHour('hours_off');\r\n"
"        selectHour('sleep_h');\r\n"
"        selectMinutes('sleep_m');\r\n"
"        selectMinutes('minutes_on');\r\n"
"        selectMinutes('minutes_off');\r\n"
"\r\n"
"\r\n"
"        byId('sleep_h').addEventListener('change',function(){\r\n"
"            sleepHour=byId('sleep_h').selectedIndex;\r\n"
"            //console.log(sleepHour);\r\n"
"            showSleep();\r\n"
"        });\r\n"
"        byId('sleep_m').addEventListener('change',function(){\r\n"
"            sleepMinute=byId('sleep_m').selectedIndex;\r\n"
"            //console.log(sleepMinute);\r\n"
"            showSleep();\r\n"
"        });\r\n"
"        byId('sleep').addEventListener('click',function(){\r\n"
"            console.log('click sleep!');\r\n"
"\r\n"
"            if(displaySleep){\r\n"
"                //Sent mode to ESP\r\n"
"                //command('&1&');\r\n"
"                showSleep();\r\n"
"            }else {\r\n"
"                hideSleep();\r\n"
"            }\r\n"
"\r\n"
"        });\r\n"
"        byId('auto').addEventListener('click',function(){\r\n"
"            if( displayAuto){\r\n"
"                //Sent mode to ESP\r\n"
"                //command('&2&');\r\n"
"                showAuto();\r\n"
"            }else{\r\n"
"                hideAuto();\r\n"
"            }\r\n"
"        });\r\n"
"        byId('manual').addEventListener('click',function(){\r\n"
"            //readStatus();\r\n"
"            if(displayManu){\r\n"
"                //Sent mode to ESP\r\n"
"                //command('&0&');\r\n"
"                showManu();\r\n"
"            }else{\r\n"
"                hideManu();\r\n"
"            }\r\n"
"\r\n"
"        });\r\n"
"        byId('on').addEventListener('click',function(){\r\n"
"            //comm&0&0&   Mode manu On\r\n"
"            command('&0&1&');\r\n"
"        });\r\n"
"        byId('off').addEventListener('click',function(){\r\n"
"            //comm&0&1   Mode manu On\r\n"
"            command('&0&0&');\r\n"
"        });\r\n"
"        byId('loader_m').addEventListener('click',function(){\r\n"
"            readStatus();\r\n"
"        });\r\n"
"        byId('loader_s').addEventListener('click',function(){\r\n"
"            readStatus();\r\n"
"        });\r\n"
"        byId('led').addEventListener('click',function() {\r\n"
"            readStatus();\r\n"
"        });\r\n"
"        byId('sleep_send').addEventListener('click',function(){\r\n"
"          sleepSend();\r\n"
"        });\r\n"
"        byClass('send').addEventListener('click',function(){\r\n"
"            var mode =byId('mode').selectedIndex;\r\n"
"            if(mode==3) return;\r\n"
"            var number=parseInt(byClass('number').innerHTML);\r\n"
"            console.log(\"Program mode:\"+mode);\r\n"
"            switch(mode){\r\n"
"                case 0:\r\n"
"                    //array[number]=new program(number);\r\n"
"                    programOff(number);\r\n"
"                    break;\r\n"
"                case 1:\r\n"
"                    hour_on=byId('hours_on').selectedIndex;\r\n"
"                    minute_on=byId('minutes_on').selectedIndex;\r\n"
"                    hour_off=byId('hours_off').selectedIndex;\r\n"
"                    minute_off=byId('minutes_off').selectedIndex;\r\n"
"                    console.log(hour_on);\r\n"
"                    console.log(minute_on);\r\n"
"                    console.log(hour_off);\r\n"
"                    console.log(minute_off);\r\n"
"                    //console.log(number);\r\n"
"                    var timeOn=hour_on*60 +minute_on;\r\n"
"                    var timeOff=hour_off*60 +minute_off;\r\n"
"                    if(isNaN(number))number=1;\r\n"
"                    //console.log(number);\r\n"
"                    programDaily(number,1,timeOn,timeOff);\r\n"
"                    //array[number]=new program(number,mode,hour_on,minute_on,hour_off,minute_off);\r\n"
"                    break;\r\n"
"//                case 2:\r\n"
"//                    hour_on=byId('hours_on').selectedIndex;\r\n"
"//                    minute_on=byId('minute_on').selectedIndex;\r\n"
"//                    hour_off=byId('hours_off').selectedIndex;\r\n"
"//                    minute_off=byId('minute_off').selectedIndex;\r\n"
"//                    console.log(hour_on);\r\n"
"//                    console.log(minute_on);\r\n"
"//                    console.log(hour_off);\r\n"
"//                    console.log(minute_off);\r\n"
"//\r\n"
"//                    //array[number]=new program(number,mode);\r\n"
"//                    break;\r\n"
"            }\r\n"
"            //console.log(array[number]);\r\n"
"        });\r\n"
"        byClass('left').addEventListener('click',function(){\r\n"
"            console.log('Click Left arrow');\r\n"
"            number--;\r\n"
"            if(number<1)number=10;\r\n"
"             //Read Status for Program with Number:number\r\n"
"            request('/status&?&2&'+number+'&');\r\n"
"            //ShowNumber();\r\n"
"        });\r\n"
"        byClass('right').addEventListener('click',function(){\r\n"
"            console.log('Click Right arrow');\r\n"
"            number++\r\n"
"            if(number>10)number=1;\r\n"
"            request('/status&?&2&'+number+'&')\r\n"
"\r\n"
"           // ShowNumber();\r\n"
"        });\r\n"
"        byId('mode').addEventListener('change',function(){\r\n"
"            hour_on=0;\r\n"
"            byId('hours_on').selectedIndex=hour_on;\r\n"
"            minute_on=0;\r\n"
"            byId('minutes_on').selectedIndex=minute_on;\r\n"
"            hour_off=0;\r\n"
"            byId('hours_off').selectedIndex=hour_off;\r\n"
"            minute_off=0;\r\n"
"            byId('minutes_off').selectedIndex=minute_off;\r\n"
"            Status();\r\n"
"        });\r\n"
"\r\n"
"function programOff(number){\r\n"
"      // mode=2 - auto  ; pr.number=1 first  ; pr.mode=0 - off  ; That is default for all program.On Status will return\r\n"
"    command('&2&'+number+'&0&');\r\n"
"}\r\n"
"function programDaily(number,mode,timeOn,timeOff){\r\n"
"      // mode=2 - auto  ; pr.number=1 first  ; pr.mode=1 - Daily  ; imteOn;timeOff That is default for all program.On Status will return\r\n"
"    command('&2&'+number+'&'+mode+'&'+timeOn+'&'+timeOff+'&');\r\n"
" }\r\n"
"        function selectHour(id){\r\n"
"            for(i=0;i<24;i++){\r\n"
"                var option=document.createElement('option');\r\n"
"                if(i<10){\r\n"
"                    option.text='0'+i;\r\n"
"                }\r\n"
"                else option.text=i;\r\n"
"                byId(id).add(option);\r\n"
"            }\r\n"
"        }\r\n"
"        function selectMinutes(id){\r\n"
"            for(i=0;i<60;i++){\r\n"
"                var option=document.createElement('option');\r\n"
"                if(i<10){\r\n"
"                    option.text='0'+i;\r\n"
"                }\r\n"
"                else option.text=i;\r\n"
"                byId(id).add(option);\r\n"
"            }\r\n"
"        }\r\n"
"        function showAuto(){\r\n"
"            byClass('programSetup').style.display='block';\r\n"
"            displayAuto=false;\r\n"
"            if(!displaySleep)hideSleep();\r\n"
"            if(!displayManu)hideManu();\r\n"
"         }\r\n"
"        function hideAuto(){\r\n"
"            byClass('programSetup').style.display='none';\r\n"
"            displayAuto=true;\r\n"
"        }\r\n"
"        function showSleep(){\r\n"
"            var now=new Date();\r\n"
"            var hour = now.getHours();\r\n"
"            var minute = now.getMinutes();\r\n"
"            byClass('sleep').style.display='block';\r\n"
"            hideAuto();\r\n"
"            hideManu();\r\n"
"            displaySleep=false;\r\n"
"        }\r\n"
"        function hideSleep(){\r\n"
"            byClass('sleep').style.display='none';\r\n"
"            displaySleep=true;\r\n"
"        }\r\n"
"        function showManu(){\r\n"
"            if(!displayAuto)hideAuto();\r\n"
"            if(!displaySleep)hideSleep();\r\n"
"            byClass('manual').style.display='block';\r\n"
"            displayManu=false;\r\n"
"\r\n"
"        }\r\n"
"        function hideManu(){\r\n"
"            byClass('manual').style.display='none';\r\n"
"            displayManu=true;\r\n"
"        }\r\n"
"        function sleepSend(){\r\n"
"            sleep=sleepHour*60+sleepMinute;\r\n"
"            command('&1&'+sleep+'&')\r\n"
"        }\r\n"
"        function ShowNumber(){\r\n"
"            byClass('number').innerHTML=number;\r\n"
"            console.log(programMode);\r\n"
"            byId('mode').selectedIndex=programMode;\r\n"
"            console.log(hour_on+':'+minute_on+';'+hour_off+':'+minute_off);\r\n"
"            byId('hours_on').selectedIndex=hour_on;\r\n"
"            byId('minutes_on').selectedIndex=minute_on;\r\n"
"            byId('hours_off').selectedIndex=hour_off;\r\n"
"            byId('minutes_off').selectedIndex=minute_off;\r\n"
"            Status();\r\n"
"        }\r\n"
"        function Status(){\r\n"
"            var mode =byId('mode').selectedIndex;\r\n"
"            byClass('left').style.visibility='visible';\r\n"
"            byClass('right').style.visibility='visible';\r\n"
"            byClass('number').style.visibility='visible';\r\n"
"            byClass('sleep').style.display='none';\r\n"
"            byClass('ON').style.display='none';\r\n"
"            byClass('OFF').style.display='none';\r\n"
"            byId('data').style.display='none';\r\n"
"            switch(mode){\r\n"
"                case 0:\r\n"
"                    break;\r\n"
"                case 1:\r\n"
"                    byClass('ON').style.display='block';\r\n"
"                    byClass('OFF').style.display='block';\r\n"
"                    byId('selectors').style.margin='10px auto';\r\n"
"                    break;\r\n"
"                case 2:\r\n"
"                    byId('data').style.display='block';\r\n"
"                    byClass('ON').style.display='block';\r\n"
"                    byClass('OFF').style.display='block';\r\n"
"                    byId('selectors').style.margin='0px auto';\r\n"
"                    break;\r\n"
"\r\n"
"            }\r\n"
"        }\r\n"
"        function espDataProgram(data){\r\n"
"            data=\"1&2&3\"+data;\r\n"
"            data = data.split('&');\r\n"
"            showAutoProgram(data);\r\n"
"        }\r\n"
"        function espData(data){\r\n"
"            data = data.split('&');\r\n"
"            var len=data.length;\r\n"
"            console.log(\"Len of data is:\"+len);\r\n"
"            if(len<2){\r\n"
"                errorStatus(data);\r\n"
"                return;\r\n"
"            }\r\n"
"            var mode=data[1];\r\n"
"            var power=data[2];\r\n"
"            setPower(power);\r\n"
"\r\n"
"            if (mode==0){\r\n"
"                console.log('Manual Mode');\r\n"
"                modeManu();\r\n"
"                //if(len>7)showAutoProgram(data);\r\n"
"            }\r\n"
"            else if(mode==1){\r\n"
"                console.log('Sleep Mode');\r\n"
"                if(len>4){\r\n"
"                    sleep=data[3];\r\n"
"                    console.log('sleep:'+sleep);\r\n"
"                    modeSleep(sleep);}\r\n"
"                //else if(len>7)showAutoProgram(data);\r\n"
"                return;\r\n"
"            }\r\n"
"            else if(mode==2){\r\n"
"                console.log('Auto Mode');\r\n"
"                if(len>4){\r\n"
"                    modeAuto(data);}\r\n"
"                else {\r\n"
"                    errorStatus(data);\r\n"
"                    return;\r\n"
"                }\r\n"
"            }\r\n"
"        }\r\n"
"        function setPower(power){\r\n"
"            console.log(\"Power status:\"+power);\r\n"
"            if(power==1){\r\n"
"                console.log(\"Power status:\"+power);\r\n"
"                byId('led').style.backgroundColor='Chartreuse';\r\n"
"            }\r\n"
"            if(power==0){\r\n"
"                console.log(\"Power status:\"+power);\r\n"
"                byId('led').style.backgroundColor='Crimson';\r\n"
"               }\r\n"
"        }\r\n"
"        function modeSleep(sleep){\r\n"
"            programPowerNumber=0;\r\n"
"            sleepHour=parseInt(sleep/60);\r\n"
"            sleepMinute=sleep - sleepHour*60;\r\n"
"            byId('sleep_m').selectedIndex=sleepMinute;\r\n"
"            byId('sleep_h').selectedIndex=sleepHour;\r\n"
"            byId('sleep').style.backgroundColor='green';\r\n"
"            byId('manual').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"            byId('auto').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"            showSleep();\r\n"
"        }\r\n"
"        function modeManu(data){\r\n"
"            byId('manual').style.backgroundColor='green';\r\n"
"            byId('sleep').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"            byId('auto').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"            sleep=0;\r\n"
"            programPowerNumber=0;\r\n"
"            byId('sleep_m').selectedIndex=0;\r\n"
"            byId('sleep_h').selectedIndex=0\r\n"
"        }\r\n"
"        function modeAuto(data){\r\n"
"            byId('auto').style.backgroundColor='green';\r\n"
"            byId('manual').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"            byId('sleep').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"            showAutoProgram(data);\r\n"
"        }\r\n"
"        function showAutoProgram(data){\r\n"
"            number=data[3];\r\n"
"            programMode=data[4];\r\n"
"            hour_on=parseInt(data[5]/60);\r\n"
"            minute_on=data[5] - hour_on*60;\r\n"
"            hour_off=parseInt(data[6]/60);\r\n"
"            minute_off=data[6] - hour_off*60;\r\n"
"            programPowerNumber=data[7];\r\n"
"            //console.log(hour_on+':'+minute_on+';'+hour_off+':'+minute_off);\r\n"
"            showAuto();\r\n"
"            ShowNumber();\r\n"
"        }\r\n"
"        function errorStatus(data){\r\n"
"            console.log('Error in status data:'+data);\r\n"
"        }\r\n"
"        function readStatus() {\r\n"
"\r\n"
"            console.log('Read status..');\r\n"
"            if(programPowerNumber!=0)\r\n"
"            {\r\n"
"                request('/status&?&2&'+programPowerNumber+'&');\r\n"
"            }\r\n"
"            request('/status?');\r\n"
"\r\n"
"        }\r\n"
"        function command(comm){\r\n"
"            console.log('Send command'+comm);\r\n"
"            //clearInterval(check);\r\n"
"            request('/comm'+comm);\r\n"
"        }\r\n"
"        function request(url){\r\n"
"            //alert(\"Call me!!\");\r\n"
"            byId('loader_m').innerHTML='Loading...';\r\n"
"            byId('loader_s').innerHTML='Loading...';\r\n"
"            byId('loader_a').innerHTML='Loading....';\r\n"
"            var xhr = new XMLHttpRequest();\r\n"
"            xhr.onreadystatechange=function(event){\r\n"
"                if(xhr.readyState === 4 && xhr.status === 200){\r\n"
"                    //alert(\"Call me from Ajax!!\");\r\n"
"                    //console.log(xhr.responseText);\r\n"
"                    //console.log(xhr.responseText.search('scanwifi='));\r\n"
"                    //console.log(xhr.responseText.search('status='));\r\n"
"//                    if(xhr.responseText.search('status&2')==0)\r\n"
"//                    {\r\n"
"//                        //Respond for auto program\r\n"
"//                        //scanWifiResult(xhr.responseText);\r\n"
"//                    }\r\n"
"                    if(xhr.responseText.search('status&')==0)\r\n"
"                    {\r\n"
"                        byId('loader_m').innerHTML='Load Ok!';\r\n"
"                        byId('loader_s').innerHTML='Load Ok!';\r\n"
"                        byId('loader_a').innerHTML='Load Ok!';\r\n"
"                        espData(xhr.responseText);\r\n"
"                    }\r\n"
"\r\n"
"                    else if(xhr.responseText.search('program&')==0)\r\n"
"                    {\r\n"
"                        byId('loader_a').innerHTML='Load Ok!';\r\n"
"                        espDataProgram(xhr.responseText);\r\n"
"                    }\r\n"
"\r\n"
"\r\n"
"\r\n"
"                } else if(xhr.readyState === 4 && xhr.status===0){\r\n"
"                    //if(url==='/scanwifi?'){byId('scan').value='Scan Error !';}\r\n"
"                   // if(url==='/status?'){\r\n"
"                        byId('led').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"                        byId('manual').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"                        byId('sleep').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"                        byId('auto').style.backgroundColor='rgba(128, 128, 128, 0.48)';\r\n"
"                        byId('loader_m').innerHTML='Load Error!';\r\n"
"                        byId('loader_s').innerHTML='Load Error!';\r\n"
"                        byId('loader_a').innerHTML='Load Error!';\r\n"
"                       // loader=false;}\r\n"
"                }\r\n"
"            }\r\n"
"            xhr.open(\"POST\",urladdress+url+Math.random(),true); //\"http://10.10.10.1\"\r\n"
"            xhr.send();\r\n"
"        }\r\n"
"\r\n"
" }\r\n"
" \r\n"
"</script>\r\n"
"\r\n"
"<style>\r\n"
"table {width:15%;margin-left:auto;margin-right:auto;}\r\n"
"th,td {display: inline-block;border-radius: 8px;}\r\n"
"th {background-color: rgba(128, 128, 128, 0.48);margin:5px;width: 300px;height:38px;font-size:30px;}\r\n"
"td {text-align:center;display:none;}\r\n"
"a {text-decoration:none;color:black;display:block;width:100%;}\r\n"
"\r\n"
"    div{-webkit-touch-callout: none;\r\n"
"        -webkit-user-select: none;\r\n"
"        -khtml-user-select: none;\r\n"
"        -moz-user-select: none;\r\n"
"        -ms-user-select: none;\r\n"
"        user-select: none;\r\n"
"        outline: 0;}\r\n"
"    body{text-align:center;padding:0;margin:0;}\r\n"
"    h1,h2 {display:inline-block;margin:10px 0 10px 0;text-align:center;border-radius:8px;cursor:pointer;background-color:rgba(128, 128, 128, 0.48);width:300px;user-select:none;}\r\n"
"    div.programSetup{display:none;}\r\n"
"    label{text-align:center;}\r\n"
"    div#ON{font-size:24px;background-color:gray;width:150px;margin:0px auto -6px auto;border-radius:8px;}\r\n"
"    select{-webkit-appearance: none;-moz-appearance: none;appearance:none;border-style:none;cursor:pointer;font-size:32px;outline:none;background-color:rgba(158, 158, 158, 0);}\r\n"
"    select::-ms-expand { display: none;}\r\n"
"    div#program{display:inline-block;padding-top:5px;background-color:rgba(158, 158, 158, 0.56);height:210px;width:170px;margin:0 auto;border-radius:8px;}\r\n"
"    ul {list-style:none;text-align:center;}\r\n"
"    li {display:inline;}\r\n"
"    .number{font-size:32px;margin:10px 0 0 0;;}\r\n"
"\r\n"
"    div.left {float:left;cursor:pointer;font-size:72px;margin:60px 0px 0px 0px;}\r\n"
"    div.right{float:right;cursor:pointer;font-size:72px;margin:60px 0px 0px 0px;}\r\n"
"    div.left:active {transform: translateX(-4px);}\r\n"
"    div.right:active {transform: translateX(+4px);}\r\n"
"\r\n"
"    button.left{float:left;cursor:pointer;font-size:48px;margin:0px 0px 0px 0px;}\r\n"
"    button.right{float:right;}\r\n"
"\r\n"
"    .send {cursor:pointer;margin:15px 0 5px 0;background-color:gray;border-radius:8px;height:40px;font-size:36px;}\r\n"
"    /*.send:active {transform: translateY(-4px);}*/\r\n"
"    #data{width:150px;border-radius:8px;margin:0px auto 9px auto;display:none;}\r\n"
"    #data:focus #mode:focus{outline:none;}\r\n"
"    .right-arrow:after {font-size:42px;content:'\\25BA';}\r\n"
"    .left-arrow:before {font-size:42px;content:'\\25C4';clear:both;}\r\n"
"    div#selectors{width:100px;height:50px;margin:0 auto;border-radius:8px;display:none;}\r\n"
"    /*div.status{margin:18px auto 0;cursor:pointer;}*/\r\n"
"    div#selectors.sleep{margin:28px auto;display:'none';}\r\n"
"    div.programSetup h3 {display:inline-block;margin:10px 0 10px 0;text-align:center;border-radius:8px;background-color:rgba(128, 128, 128, 0.48);width:200px;user-select:none;}\r\n"
"    div.manual{display:none;}\r\n"
"    div.button{cursor:pointer;padding: 20px;margin:10px 0 0px 0;background-color:gray;border-radius:8px;height:50px;font-size:36px;}\r\n"
"    #off.button{margin:30px 0 0 0;}\r\n"
"    div.button:active {transform: translateY(-4px);}\r\n"
"</style>\r\n"
"\r\n"
"<body>\r\n"
"<table>\r\n"
"<tr>\r\n"
"  <th style='background-color:white;' >PROGRAM</th>\r\n"
"</tr>\r\n"
"\r\n"
"<tr>\r\n"
"<th id='led'>LED</th>\r\n"
"</tr>\r\n"
"\r\n"
"<tr>\r\n"
" <th id='manual'> MANUAL </th>\r\n"
"	<td class='manual' style='width:200px;margin:0 auto;'>\r\n"
"	  <div id='on' class='button'>ON</div>\r\n"
"		<div id='off' class='button' >OFF</div>\r\n"
"	  <div id='loader_m' style=\"margin:10px auto;cursor:pointer;\">Loading...</div>\r\n"
"	</td>\r\n"
"</tr>\r\n"
"\r\n"
"<tr>\r\n"
"<th id='auto'> AUTO </th>\r\n"
"<td class='programSetup'>\r\n"
"    <h3> PROGRAM SETUP </h3>\r\n"
"    <div  style='text-align:center;margin:0 auto; width:300px;'>\r\n"
"        <div class=\"left\">&#171;</div>\r\n"
"        <div id='program' >\r\n"
"            <select id='mode' >\r\n"
"                <option val='0'>None</option>\r\n"
"                <option val='1'>Daily</option>\r\n"
"                <!--  <option val='2'>Date</option>-->\r\n"
"                <!--<option val='3'>Sleep</option>-->\r\n"
"            </select>\r\n"
"            <input id='data' type='date'></input>\r\n"
"            <div id='selectors' class='ON' >\r\n"
"                <span style='display:inline-block; margin:2px'>ON</span>\r\n"
"                <br>\r\n"
"                <select id='hours_on'></select><span style='font-size:24px; vertical-align:text-bottom;'>:</span><select id='minutes_on'></select>\r\n"
"            </div>\r\n"
"            <br>\r\n"
"\r\n"
"            <div id='selectors' class='OFF' >\r\n"
"                <span style='display:inline-block;margin:2px'>OFF</span>\r\n"
"                <br>\r\n"
"                <select  id='hours_off'></select><span style='font-size:24px; vertical-align:text-bottom;'>:</span><select id='minutes_off'></select>\r\n"
"            </div>\r\n"
"        </div>\r\n"
"        <div class=\"right\">&#187;</div>\r\n"
"    </div>\r\n"
"    <div style='width:200px;margin:0 auto;'>\r\n"
"        <div class='number'></div>\r\n"
"        <div class='send'>Send</div>\r\n"
"        <div id='loader_a'>Loading...</div>\r\n"
"    </div>\r\n"
"</td>\r\n"
"\r\n"
"\r\n"
"</tr>\r\n"
"\r\n"
"<tr>\r\n"
"<th id='sleep'> SLEEP </th>\r\n"
" <td class='sleep' id='selectors' style='margin: 0 auto 20px;font-size:24px;'>\r\n"
"    <!--<span style='display:inline-block;font-size:18px' >On</span>-->\r\n"
"    <!--<span id='sleep_on'>00:00</span>-->\r\n"
"    <span>Off after</span>\r\n"
"    <!--\r\n"
"    <span id='sleep_off' style=' font-weight: 600;'> 00:00</span>\r\n"
"    -->\r\n"
"    <br>\r\n"
"    <select id='sleep_h'></select><span style='font-size:32px; vertical-align:text-bottom;'>:</span><select id='sleep_m'></select>\r\n"
"    <br>\r\n"
"    <div id='sleep_send' class='send'>Send</div>\r\n"
"    <div id='loader_s'>Loading...</div>\r\n"
" </td>\r\n"
"</tr>\r\n"
"\r\n"
"<tr>\r\n"
"  <th>\r\n"
"   <a href=\"./\">\r\n"
"    SETUP\r\n"
"   </a>\r\n"
"  </th>\r\n"
"</tr>\r\n"
"\r\n"
"</table>\r\n"
"</body>\r\n"
"</html>\r\n"
"\r\n"
"\r\n"
"\r\n"