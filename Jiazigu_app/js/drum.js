/**
Copyright 2012 Google Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Author: Renato Mangini (mangini@chromium.org)
Author: Luis Leao (luisleao@gmail.com)
Author: Ken Rockot (rockot@chromium.org)
**/

const SENSOR_REFRESH_INTERVAL=200;

//ASCII-BASED protocol definition
const CMD_START_CHAR      ='.';
const CMD_END_CHAR        =';';
const CMD_SEPARE_CHAR     =',';
                           
const RSP_OK              ='0';
const RSP_ERROR_LENGTH    ='1';
const RSP_ERROR_CHECKSUM  ='2';
const RSP_ERROR_BUSY      ='3';
const RSP_ERROR_PARAMETER ='4';
                           
const CMD_WORD_SETSERVO   ='A';
const CMD_WORD_GETSERVO   ='B';
const CMD_WORD_SETDRUM    ='C';
const CMD_WORD_GETDRUM    ='D';
const CMD_WORD_SETSONG    ='E';
const CMD_WORD_GETSONG    ='F';
const CMD_WORD_RUN        ='G';
const CMD_WORD_STOP       ='H';
const CMD_WORD_SETCONFIG  ='I';
const CMD_WORD_GETCONFIG  ='J';
const CMD_WORD_HEARTBEAT  ='K';
const CMD_WORD_LOADEEPROM ='L';
const CMD_WORD_STOREEEPROM='M';
const CMD_WORD_CLEAREEPROM='N';

const MAX_POS_ONE_LINE=95;
const SHOW_LINES = 6;
const MAX_TONE = 10;

const TONE_INDEX_DG = 0;
const TONE_INDEX_JG = 1;
const TONE_INDEX_GT = 2;
const TONE_INDEX_ZT = 3;
const TONE_INDEX_DT = 4;
const TONE_INDEX_KC = 5;
const TONE_INDEX_BC = 6;
const TONE_INDEX_DC = 7;
const TONE_INDEX_DD = 8;
const TONE_INDEX_KB = 9;


(function() {
  var btnOpen = document.querySelector(".open");
  var btnClose = document.querySelector(".close");
  var logArea = document.querySelector(".log");
  var statusLine = document.querySelector("#status");
  var serialDevices = document.querySelector(".serial_devices");
  var connection = null;
  
/*jiazigu communication*/		
	var cmd_position = 0;
	var cmd_state = 0;
	var response_buffer = new Int32Array(10);
  var cmd_buffer = new Uint8Array(256);
	

	var updown = [0,0,0,0];
	var drumselected = -1;
	
	//song
	var	edit_line = 0;
	var edit_pos = 0;
	var start_line = 0;
	var max_line = SHOW_LINES - 1;
	var tone_compensate = [84,60,52,56,68,51,51,46,55,92];
	var PNGs =  [
      "url(../img/yinbiao_gu_down.png)", 
      "url(../img/yinbiao_gu_up.png)" ,
      "url(../img/yinbiao_gu_up.png)" ,
      "url(../img/yinbiao_gu_up.png)" ,
      "url(../img/yinbiao_gu_up.png)" ,
      "url(../img/yinbiao_ca.png)", 
      "url(../img/yinbiao_ca_cycle.png)", 
      "url(../img/yinbiao_ca_six.png)", 
      "url(../img/yinbiao_ca.png)", 
      "url(../img/yinbiao_ca.png)"      
  ];
	
	
//file system	
	var openFileButton = document.querySelector('#openfile');
	var newFileButton = document.querySelector('#new');
	var saveFileButton = document.querySelector('#save');
	var saveasFileButton = document.querySelector('#saveas');
	var fileChanged=false;
	var chosenEntry = null;
	var output = document.querySelector('output');
	
  var leftButton = document.querySelector('#left'); 
  var rightButton = document.querySelector('#right');
  var upButton = document.querySelector('#up'); 
  var downButton = document.querySelector('#down'); 
  var deleteButton = document.querySelector('#delete');
  
  var runButton = document.querySelector('#run'); 
  var stopButton = document.querySelector('#stop'); 
	

//log	
  var logObj = function(obj) {
    console.log(obj);
  }

  var logSuccess = function(msg) {
    log("<span style='color: green;'>" + msg + "</span>");
  };

  var logError = function(msg) {
    statusLine.className = "error";
    statusLine.textContent = msg;
    log("<span style='color: red;'>" + msg + "</span>");
  };

  var log = function(msg) {
    console.log(msg);
    logArea.innerHTML = msg + "<br/>" + logArea.innerHTML;
  };

//dislog
	var dialog = document.querySelector('#dialog1');
	var initDialogListeners = function(){
		document.querySelector('#save_it').addEventListener("click", function(evt) {
		  dialog.close("save");
		});
		document.querySelector('#discard_it').addEventListener("click", function(evt) {
		  dialog.close("discard");
		});
		document.querySelector('#cancel_it').addEventListener("click", function(evt) {
		  dialog.close("canceled");
		});
			
		dialog.addEventListener("close", function(evt) {
			switch(dialog.returnValue){
				case "save":
					if(null != chosenEntry){
						//save to the exist entry
						logSuccess("save exist file");
						saveFileEntry();
					}else{
						//save as
						logSuccess("save as new file");
					}
				break;
				case "discard":
					//new file
					clearAllTones();
    			fileChanged = false;
    			chosenEntry = null;
				break;
				case "canceled":
				//nothing to do
				break;
			}
		});
	
		// called when the user Cancels the dialog, for example by hitting the ESC key
		dialog.addEventListener("cancel", function(evt) {
			dialog.close("canceled");
		});
	}
	
	var flagenabled=false;
	var flashflag=function(){
		if(flagenabled){
			document.getElementById("pu_flag").style.display = "none";
		}else{
			document.getElementById("pu_flag").style.display = "block";
		}
		flagenabled = !flagenabled;
	};
	
	var t1 = window.setInterval(flashflag,500); 


  var changeTab = function() {
    var _drum = document.querySelector("#drumsec");
    var _song = document.querySelector("#songsec");
    if (window.location.hash === "#songlink") {
      _song.className = "";
      _drum.className = "hidden";
    } else {
      _drum.className = "";
      _song.className = "hidden";
    }
  };
  
	function errorHandler(e) {
	  console.error(e);
	}  
	
	function readAsText(fileEntry, callback) {
	  fileEntry.file(function(file) {
	    var reader = new FileReader();
	
	    reader.onerror = errorHandler;
	    reader.onload = function(e) {
	      callback(e.target.result);
	    };
	
	    reader.readAsText(file);
	  });
	}

	function loadFileEntry(_chosenEntry) {
	  chosenEntry = _chosenEntry;
	  
	  chrome.fileSystem.getDisplayPath(chosenEntry, function(path) {
	  	statusLine.textContent = "JiaZiGu - " + path;
    	statusLine.className = "";
    });
        
    readAsText(chosenEntry, function(result) {
      logSuccess(result);
    });
	}

	function waitForIO(writer, callback) {
	  // set a watchdog to avoid eventual locking:
	  var start = Date.now();
	  // wait for a few seconds
	  var reentrant = function() {
	    if (writer.readyState===writer.WRITING && Date.now()-start<4000) {
	      setTimeout(reentrant, 100);
	      return;
	    }
	    if (writer.readyState===writer.WRITING) {
	      console.error("Write operation taking too long, aborting!"+
	        " (current writer readyState is "+writer.readyState+")");
	      writer.abort();
	    } 
	    else {
	      callback();
	    }
	  };
	  setTimeout(reentrant, 100);
	}
	
	function saveFileEntry() {
		logSuccess("saveFileEntry");
		if(null == chosenEntry) return;
		
//  	var config = {type: 'saveFile', suggestedName: chosenEntry.name};
  	
//  	chrome.fileSystem.chooseEntry(config, function(writableEntry) {
  		logSuccess("entryname:"+chosenEntry.name);
  		
    	var blob = new Blob(["Hello World!"],{type: 'text/plain'});
  		logSuccess("blob size"+blob.size);
    	
    	chosenEntry.createWriter(function(writer) {
	
	    writer.onerror = errorHandler;
	    writer.onwriteend = function(e){
	    	logSuccess('Write complete :');};
	
      writer.truncate(blob.size);
      waitForIO(writer, function() {
        writer.seek(0);
        writer.write(blob);
      });
	  }, errorHandler);
//    });
  };
  	
	var update_pu_flag_position = function(){
    	document.getElementById("pu_flag").style.top=(85*(edit_line-start_line)+35)+"px";
			document.getElementById("pu_flag").style.left=(edit_pos*13+18)+"px";
	}
	
	var addNewTone=function(tone_id){
   	  var divobj = document.createElement("div");
			divobj.id = "tone" + edit_line + "p" + edit_pos + "t" + tone_id;;
	   	divobj.style.position="absolute";
			divobj.style.overflow="hidden"; 
    	divobj.style.left = (edit_pos*13+26)+"px";
    	divobj.style.top = ((edit_line-start_line)*85+tone_compensate[tone_id])+"px";
			divobj.style.width="12px"; 
			divobj.style.height="19px"; 
			divobj.style.backgroundRepeat="no-repeat"; 
			divobj.style.backgroundImage=PNGs[tone_id]; 
			document.querySelector(".song").appendChild(divobj);  
			
			if(edit_line>max_line) max_line = edit_line;
		}

	var creatOneLable=function(line,pos){
 	  var divobj = document.createElement("div");
		divobj.id = "labeldiv" + (line * 4 + pos);
   	divobj.style.position="absolute";
		divobj.style.overflow="hidden"; 
  	divobj.style.left = (pos*312+180)+"px";
  	divobj.style.top = (line*85+100)+"px";
		divobj.style.width="18px"; 
		divobj.style.height="15px"; 
		divobj.style.backgroundRepeat="no-repeat"; 
		divobj.innerHTML = (line * 4 + pos + 1); 
		divobj.style.color="grey";
		document.querySelector(".song").appendChild(divobj);  
	}
			
	var creatLables=function(){
		for(var i=0;i<6;i++){
			for(var j=0;j<4;j++){
			creatOneLable(i,j);
			}
		}
	}

	var clearAllTones = function(){
		var count = 0;
		logSuccess("delete elements");
		for(var line=0;line<=max_line;line++){
			for(var pos=0;pos<=MAX_POS_ONE_LINE;pos++){
				for(tone=0;tone<MAX_TONE;tone++){
					var objname="tone" + line + "p" + pos + "t" + tone;
					var divobj = document.getElementById(objname);
						if(null != divobj){
							count ++;
							document.querySelector(".song").removeChild(divobj);
							}
						}
					}
				}
			logSuccess(":"+count);
			start_line = 0;
			edit_line = 0;
			edit_pos = 0;
			max_line = SHOW_LINES - 1;
			update_pu_flag_position();
			for(var i=0;i<SHOW_LINES;i++){
				for(var j=0;j<4;j++){
				document.getElementById("labeldiv" + (i * 4 + j)).innerHTML = (i * 4 + j + 1); 
				}
			}	
	};
	
	var updateLabels=function(){
		//update labels
		for(var i=0;i<SHOW_LINES;i++){
			for(var j=0;j<4;j++){
			document.getElementById("labeldiv" + (i * 4 + j)).innerHTML = ((start_line+i) * 4 + j + 1); 
			}
		}
		
		for(var line=0;line<=max_line;line++){
			for(var pos=0;pos<=MAX_POS_ONE_LINE;pos++){
				for(tone=0;tone<MAX_TONE;tone++){
					var objname="tone" + line + "p" + pos + "t" + tone;
					var divobj = document.getElementById(objname);
						if(null != divobj){
							if(line<start_line || line>start_line+SHOW_LINES-1){
								divobj.style.display = "none";
							}else{
								divobj.style.display = "block";
								divobj.style.top = ((line-start_line)*85+tone_compensate[tone])+"px";
							}
						}
					}
				}
			}
	}

  var init = function() {
    if (!serial_lib)
      throw "You must include serial.js before";

    enableOpenButton(true);
    btnOpen.addEventListener("click", openDevice);
    btnClose.addEventListener("click", closeDevice);
    window.addEventListener("hashchange", changeTab);
    document.querySelector(".refresh").addEventListener("click", refreshPorts);
    initDrumListeners();
    refreshPorts();
    document.querySelector("#songsec").addEventListener("click",songclick);
		initDialogListeners();
		
		creatLables();
		update_pu_flag_position();

		openFileButton.addEventListener('click', function(e) {
		  var accepts = [{
		    mimeTypes: ['text/*'],
		    extensions: ['dru']
		  }];
		  chrome.fileSystem.chooseEntry({type: 'openFile', accepts: accepts}, function(theEntry) {
		    if (!theEntry) {
		      logSuccess('No file selected.');
		      return;
		    }
		    // use local storage to retain access to this file
		    chrome.storage.local.set({'chosenFile': chrome.fileSystem.retainEntry(theEntry)});
		    loadFileEntry(theEntry);
		  });
		});
		
    newFileButton.addEventListener('click', function(e) {
    	logSuccess("newFile");
    	if(fileChanged){
	  		dialog.showModal();
    	}else{
    		clearAllTones();
    		fileChanged = false;
    		chosenEntry = null;
    	}
    	});
    
    
    saveFileButton.addEventListener('click', function(e) {});
    saveasFileButton.addEventListener('click', function(e) {});
    
    leftButton.addEventListener('click', function(e) {
    	if(0==edit_pos){
    		if(0==edit_line){
    			//nothing to do
    		}else{
    			edit_line--;
    			edit_pos=MAX_POS_ONE_LINE;
    			
	  			if(edit_line<start_line){
	  				start_line--;
	  				updateLabels();
	  			}
	    		update_pu_flag_position();
    		}
    	}else{
    		edit_pos--;
    		update_pu_flag_position();
    	}
    	});
    	
    rightButton.addEventListener('click', function(e) {
    	if(MAX_POS_ONE_LINE==edit_pos){
    		//to the most right pos
  			edit_line++;
  			edit_pos=0;
   			    		
   			if(edit_line>=start_line+SHOW_LINES){
   				start_line++;
   				updateLabels();
   			}
    		update_pu_flag_position();
   			
    	}else{
    		edit_pos++;
    		update_pu_flag_position();
    	}    	
    	});
    
    upButton.addEventListener('click', function(e) {
   		if(edit_line>0){
  			edit_line--;
  			if(edit_line<start_line){
  				start_line--;
  				updateLabels();
  			}else{
    			update_pu_flag_position();
    		}
  		}
    	});
    	 	    
    downButton.addEventListener('click', function(e) {
   			edit_line++;
   			if(edit_line>=start_line+SHOW_LINES){
   				start_line++;
   				updateLabels();
   			}else{
    			update_pu_flag_position();
   			}
    	});

    deleteButton.addEventListener('click', function(e) {
    	for(var i=0;i<MAX_TONE;i++){
 				var objname="tone" + edit_line + "p" + edit_pos + "t" + i;
  			var divobj=document.getElementById(objname);
   			if(null != divobj){
   				document.querySelector(".song").removeChild(divobj);
   				fileChanged = true;
   			}	   		
    	}
    	
    	});
    	
    runButton.addEventListener('click', function(e) {});
    stopButton.addEventListener('click', function(e) {});
		
  };

	var songclick = function(){
		var line=0;
		var xiaoshu=0;
		line=Math.floor((event.clientY)/85);
		
		xiaoshu=(event.clientY)/85-line;
		
		if(line>0 && line<7){
			if(xiaoshu > 0.1 && xiaoshu < 0.9){
				//move the pu range element
				edit_line = start_line + line-1;
				edit_pos = Math.floor((event.clientX+8)/13)-1;
  			update_pu_flag_position();
			}
		}
	}
	
  var addListenerToElements = function(eventType, selector, listener) {
    var addListener = function(type, element, index) {
      element.addEventListener(type, function(e) {
        listener.apply(this, [e, index]);
      });
    };
    var elements = document.querySelectorAll(selector);
    for (var i = 0; i < elements.length; ++i) {
      addListener(eventType, elements[i], i);
    }
  };
  
  var initDrumListeners = function() {
  	addListenerToElements("click",".tone_buttons div",function(e,index)
  	{
  		var objname="tone" + edit_line + "p" + edit_pos + "t" + index;
  		if(document.getElementById(objname) != null) return; //same tone exist, return directly
  		
  		var handcount=0;
  		var leftfoot = false;
 		
  		for(var i=TONE_INDEX_JG;i<=TONE_INDEX_DD;i++)
  		{
  			objname="tone" + edit_line + "p" + edit_pos + "t" + i;
	
  			if(document.getElementById(objname) != null) 
  			{
  				handcount++;
  				if((TONE_INDEX_BC == i) || (TONE_INDEX_KC == i)) leftfoot = true;
  			}
  		}
  		
  		objname="tone" + edit_line + "p" + edit_pos + "t" + TONE_INDEX_KB;
  		if(document.getElementById(objname) != null) leftfoot = true;
  		
 			if((TONE_INDEX_KC == index) || (TONE_INDEX_BC == index) || (TONE_INDEX_KB == index))
  		{
				if(leftfoot) return;
  		}
  		
  		if((index >= TONE_INDEX_JG) && (index <= TONE_INDEX_DD))
  		{
  			logSuccess("handcount:"+handcount);
  			if(handcount>1) return;
  		}
  		
  		logSuccess("addNewTone" + index);
 			addNewTone(index);
 			fileChanged = true;
   		
  	});
  	
	  addListenerToElements("change", ".lefthand input[type='range']", function(e, index) {
	      this.nextSibling.textContent = this.value;
	      sendsetmotor(index,this.value);
	      if(drumselected >= 0) {
	      	sendsetdrum(drumselected,updown[0],index,this.value);
	      }
	  });
	  
	  addListenerToElements("change", ".righthand input[type='range']", function(e, index) {
	      this.nextSibling.textContent = this.value;
	      sendsetmotor(index+4,this.value);
	      if(drumselected >= 0) {
	      	sendsetdrum(drumselected+8,updown[1],index+4,this.value);
	      }
	  });
	  
	  addListenerToElements("change", ".leftfoot input[type='range']", function(e, index) {
	      this.nextSibling.textContent = this.value;
	      sendsetmotor(8,this.value);
	     	sendsetdrum(16,updown[2],0,this.value);    
	     	});
	     	
	  addListenerToElements("change", ".rightfoot input[type='range']", function(e, index) {
	      this.nextSibling.textContent = this.value;
	      sendsetmotor(12,this.value);
	     	sendsetdrum(24,updown[3],0,this.value);  
	  });    
	  
	  addListenerToElements("click", ".updowns button", function(e, index) {
	    if (this.classList.contains("on")) {
	      // turn it off
	      this.classList.remove("on");
	      this.textContent = "Down";
	      updown[index]=0;
	    } else {
	      // turn it on
	      this.classList.add("on");
	      this.textContent = "Up";
	      updown[index]=1;
	    }
	    if(index==0) {
	    //left hand
	    	if (drumselected >= 0) {
	    		sendgetdrum(drumselected,updown[index]);
	    	}
	  	}else if (index == 1) {
	  		//right hand
	    	if (drumselected >= 0) {
	    		sendgetdrum(drumselected+8,updown[index]);
	    	}
	    } else if (index == 2) {
	  		//left foot
	  		sendgetdrum(16,updown[index]);
	  	} else if (index == 3) {
	  		//right foot
	  		sendgetdrum(24,updown[index]);
	  	}
	  });
	
	  addListenerToElements("click", ".drums button", function(e, index) {
	 	var elements = document.querySelectorAll(".drums button");
	  	for (var i = 0; i < elements.length; i++) {
	  		if(i!=index) {
	  			//turn all others to off
	  			if (elements[i].classList.contains("on")) elements[i].classList.remove("on");
	  		}
	  	}
	 	
		  if (this.classList.contains("on")) {
	  		// turn it off
	  		this.classList.remove("on");
	  		logError("No drum selected!");
	  		drumselected = -1;
			} else {
	  		// turn it on
	  		this.classList.add("on");
	  		//logError("Drum " + index + " selected");
	  		drumselected = index;
	  		sendgetdrum(drumselected,updown[0]);
	  		sendgetdrum(drumselected+8,updown[1]);
			}
	  });
	};
	
	var sendsetmotor = function(motor_id,value)
	{
		var msg = CMD_START_CHAR + CMD_WORD_SETSERVO + motor_id + CMD_SEPARE_CHAR + value + CMD_END_CHAR;
		sendSerial(msg);

	}
	
  var sendsetdrum = function(drum_id,updown,motor_id,value){
		var msg = CMD_START_CHAR + CMD_WORD_SETDRUM + 
							drum_id + CMD_SEPARE_CHAR + 
							updown + CMD_SEPARE_CHAR +
							motor_id + CMD_SEPARE_CHAR + 
							value + CMD_END_CHAR;
							
		sendSerial(msg);

  }

	var sendgetdrum = function(drum_id,updown,motor_num)
	{
		var msg = CMD_START_CHAR + CMD_WORD_GETDRUM + 
							drum_id + CMD_SEPARE_CHAR + 
							updown + CMD_END_CHAR;
							
		sendSerial(msg);		
	}


  var toHexString = function(i) {
    return ("00" + i.toString(16)).substr(-2);
  };

  var enableOpenButton = function(enable) {
    btnOpen.disabled = !enable;
    btnClose.disabled = enable;
  };

  var refreshPorts = function() {
    while (serialDevices.options.length > 0)
      serialDevices.options.remove(0);

    serial_lib.getDevices(function(items) {
      logSuccess("got " + items.length + " ports");
      for (var i = 0; i < items.length; ++i) {
        var path = items[i].path;
        serialDevices.options.add(new Option(path, path));
        if (i === 1 || /usb/i.test(path) && /tty/i.test(path)) {
          serialDevices.selectionIndex = i;
          logSuccess("auto-selected " + path);
        }
      }
    });
  };

  var openDevice = function() {
    var selection = serialDevices.selectedOptions[0];
    if (!selection) {
      logError("No port selected.");
      return;
    }
    var path = selection.value;
    statusLine.classList.add("on");
    statusLine.textContent = "Connecting";
    enableOpenButton(false);
    serial_lib.openDevice(path, onOpen);
  };

  var onOpen = function(newConnection) {
    if (newConnection === null) {
      logError("Failed to open device.");
      return;
    }

    connection = newConnection;
    connection.onReceive.addListener(onReceive);
    connection.onError.addListener(onError);
    connection.onClose.addListener(onClose);
    logSuccess("Device opened.");
    enableOpenButton(false);
    statusLine.textContent = "Connected";
  };

  var sendSerial = function(message) {
  if (connection === null) {
    return;
  }
  if (!message) {
    logError("Nothing to send!");
    return;
  }
  if (message.charAt(message.length - 1) !== '\n') {
    message += "\n";
  }     
  
  //log message and send it out
  logSuccess(message);
  connection.send(message);
  };

  var onError = function(errorInfo) {
//  	logError("Error");
    if (errorInfo.error !== 'timeout') {
      logError("Fatal error encounted. Dropping connection.");
      closeDevice();
    }
  };

	var getresponse = function(){
		 //check command
  var cmd = cmd_buffer[0];
	var parameters = new Uint32Array(10);
	var count = 0;
	parameters[0] = 0;
	
  for (var pos = 1; pos < cmd_position; pos++) {
    var thischar = cmd_buffer[pos];
    if (thischar == 44) {
      //parameter seperater
      count ++;
      parameters[count] =  0;
    }
    else if ((thischar >= 48) && (thischar <= 57)) {
      //valid data
      parameters[count] = parameters[count] * 10 + (thischar - 48);
    }
	}
	count++;
	
	if(cmd == 66){  //get motor
		
	}
//logSuccess("cmd " + cmd + "para1 " + parameters[1] + "count " + count);	
	if(cmd == 68) {
		if(parameters[1] < 8){
			if(count == 7){
				
				for(var i=0;i<4;i++){
	    					document.querySelectorAll(".lefthand input[type='range']")[i].value = parameters[i+3];
	    					document.querySelectorAll(".lefthand input[type='range']")[i].nextSibling.textContent = parameters[i+3];				
				}
			}
		}else if(parameters[1]) { 
			if(count == 7){
				for(var i=0;i<4;i++){
	    					document.querySelectorAll(".righthand input[type='range']")[i].value = parameters[i+3];
	    					document.querySelectorAll(".righthand input[type='range']")[i].nextSibling.textContent = parameters[i+3];				
				}
			}
		}		
		if(parameters[1] == 16 && count == 4){
    					document.querySelectorAll(".leftfoot input[type='range']")[0].value = parameters[3];
    					document.querySelectorAll(".leftfoot input[type='range']")[0].nextSibling.textContent = parameters[i+3];				
		}				
		if(parameters[1] == 24 && count == 4){
    					document.querySelectorAll(".rightfoot input[type='range']")[0].value = parameters[3];	
    					document.querySelectorAll(".rightfoot input[type='range']")[0].nextSibling.textContent = parameters[i+3];			
		}				
	}
	}

  var onReceive = function(data) {
  	logError(data.toString());
  	for(var i=0;i<data.length;i++){

			switch(cmd_state){
				case 0:  //command not start
				    if (data.charAt(i) == CMD_START_CHAR) {
	          //start command
	          cmd_position = 0;
	          cmd_state = 1;
	        }
	        break;
	        
				case 1: //command started
	        if (data.charAt(i) == CMD_START_CHAR) {
	          //restart the command
	          cmd_position = 0;
	          cmd_state = 1;
	        } else if (data.charAt(i) == CMD_END_CHAR) {
	          //got command end char
	          getresponse();
	          cmd_state = 0;
	        } else {
	          cmd_buffer[cmd_position] = data.charCodeAt(i);
	          cmd_position++;
	          if (cmd_position >= 240) {
	            cmd_state = 0;
	          }
	        }				
				break;
				
				default:
				break;
				
			}
		}

  };

  var closeDevice = function() {
   if (connection !== null) {
     connection.close();
   }
  };

  var onClose = function(result) {
    connection = null;
    enableOpenButton(true);
    statusLine.textContent = "Hover here to connect";
    statusLine.className = "";
  }

  init();
})();



