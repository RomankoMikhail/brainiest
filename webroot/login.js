import {addr, ws} from './addrs.js';
window.onload = function(){
	new Vue({ 
		el: '#logform',
		data:{
			session_id: "",
			username: "",
			dispLogin: false,
			dispSession: true,
			dispNickname: false,
		},
		methods:{
			getSession : function(event){
				console.log("got session");
				this.session_id = event.target.parentElement.children[0].value;
				if (this.session_id.length > 0) {
					this.dispSession = false;
					this.dispNickname = true;
				}
			},
			getName : function(event){
				console.log("got name");
				this.username = event.target.parentElement.children[0].value;
				if (this.username.length > 2) {
					this.dispNickname = false;
					localStorage.logged = this.username;
					localStorage.logType = "player";
					localStorage.session_id = this.session_id;
					window.location.href = addr+"index.html";
				}
			},
			createSession : function(){
				console.log("dir to login");
				this.dispSession = false;
				this.dispNickname = false;
				this.dispLogin = true;
			},
			back : function(){
				console.log("login retreat");
				this.dispSession = true;
				this.dispNickname = false;
				this.dispLogin = false;
			},
			log : function(event){
				console.log("login attempt");
				this.username = event.target.parentElement.previousElementSibling.previousElementSibling.value;
				console.log(this.username);
				console.log(event.target.parentElement.previousElementSibling.value);
				if (this.username.length > 2) {
					this.dispNickname = false;
					localStorage.logged = this.username;
					localStorage.logType = "admin";
					localStorage.session_id = "";
					window.location.href = addr+"index.html";
				}
			}
		}
});

}
