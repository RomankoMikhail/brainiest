
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
				session_id = event.target.parentElement.children[0].value;
				if (session_id.length > 0) {
					this.dispSession = false;
					this.dispNickname = true;
				}
			},
			getName : function(event){
				console.log("got name");
				username = event.target.parentElement.children[0].value;
				if (username.length > 2) {
					this.dispNickname = false;
					localStorage.logged = username;
					localStorage.logType = "player";
					localStorage.session_id = session_id;
					window.location.href = "http://192.168.0.106:8080/index.html";
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
				username = event.target.parentElement.previousElementSibling.previousElementSibling.value;
				console.log(username);
				console.log(event.target.parentElement.previousElementSibling.value);
				if (username.length > 2) {
					this.dispNickname = false;
					localStorage.logged = username;
					localStorage.logType = "admin";
					localStorage.session_id = "";
					window.location.href = "http://192.168.0.106:8080/index.html";
				}
			}
		}
});

}
