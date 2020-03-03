import {addr, ws} from './addrs.js';

var socket;

var testRnd3 = {
	type:"3",
	data: 	{
		gridColumns:['col1','col2','col3','col4','col5'],
		gridData:[	
					["1","2","3","4","5"],
					["6","7","8","9","10"],
					["11","12","13","14","15"],
					["16","17","18","19","20"],
					["21","22","23","24","25"],
				]
			}
	
};
var testRnd1 = {

	type:"1",
	data: 	{

				question: "Пробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопросПробный вопрос",
				answers: [["Ответ №1", "Ответ №2", "Ответ №3"],["Ответ №3","Ответ №3","Ответ №3"],["Ответ №3","Ответ №3","Ответ №3"],["Ответ №3","Ответ №3","Ответ №3"],["Ответ №3"]]
			}
};
var resetJson = {
	type: "0",
};

Vue.component('rd3-quest-grid', {
  template: '#rd3-grid-template',
  props: {
    quests: Array,
    columns: Array,
	type: String,
  },
  computed: {
    getQuests: function () {
      var quests = this.quests;
	console.log(quests);
      return quests
    },
  },
  methods: {
      pressed : function(event) {
        if (event) {
		console.log("y = " + event.target.id);
		console.log("x = " + event.target.cellIndex);
        console.log(event);
        }
      }
    }
})

Vue.component('rd1-quest-grid', {
	template: '#rd1-grid-template',
	props: {
		count:Number,
		question: String,
		answers: Array,
	},
	methods: {
      pressed : function(event) {
        if (event) {
		console.log("y = " + event.target.id);
		console.log("x = " + event.target.cellIndex);
        console.log(event);
        }
      },
      getObj: function(event){
      	console.log(event);
      },
      addCount: function(){
      	this.count = this.count + 1;
      }
    }
})

window.onload = function () {

	if (localStorage.logged == undefined) {
		window.location.href = addr+"login.html";
	}

	console.log($("#profileTop > span"));
	$("#profileTop > span").text(localStorage.logged);
	
	console.log("socks loaded");

    var quest = new Vue({
    el: '#rd3',
    data: {
		json: {}
    },
  	});
	
	var vict = new Vue({
		el: '#rd1',
		data:{
			count:0,
			json: {}
		},
		updated: function(){
			this.$nextTick(function () {
				let title = document.getElementById("rd1-title");
				let table = document.getElementById("rd1-body");
				document.getElementById("rd1-title").style.width = table.offsetWidth-75+"px";
				console.log(document.getElementById("rd1-title").style);
				

			});
		}
	});
				


	socket = new WebSocket(ws);
	socket.onopen  = function(e) {
		console.info("Open socket");
	}
	socket.onmessage = function(event) {
		console.log(JSON.parse(event.data));
		let locJson = JSON.parse(event.data);
		switch(locJson.type){
			case '1':
					{
						vict._data.json = locJson.data;
						console.log(vict._data.json);
						quest._data.json = "";
						break;
					}
			case '3': 
					{
						quest._data.json = locJson.data;
						console.log(quest._data.json);
						vict._data.json = "";
						break;
					}					
			case '0': 
					{
						vict._data.json = "";
						quest._data.json = "";
						break;
					}
			default:
			{
				console.log(event);
			}
		} 
	}
	socket.onerror = function(error) {
		console.log(error.data);
	}

let controller = new Vue({
	el: '#controller',
	methods: {
		test1: function(){
			socket.send(JSON.stringify(testRnd1));
			
		},
		test3: function(){
			socket.send(JSON.stringify(testRnd3));
		},
		test: function(){
			console.log("vict happened");
			console.log(vict._data);
			console.log("quest happened");
			console.log(quest._data);

		},
		reset: function(){
			socket.send(JSON.stringify(resetJson));
		},
		exit: function(){
			localStorage.logged = undefined;
			window.location.href = addr+"login.html";
		}
	}

});

}

$(window).scroll(function() {
	if ($(this).scrollTop() > 1) {
		$('header').addClass("sticky");
		$("#bodyClass").addClass("indentCollapsed");
	} else {
		$('header').removeClass("sticky");
		$("#bodyClass").removeClass("indentCollapsed");
	}
});









