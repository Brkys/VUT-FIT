function stickMenu(nav, sticky) {
if (window.pageYOffset >= sticky)
	nav.classList.add("sticky")
else
	nav.classList.remove("sticky");
}

function saveDialog() {
	var name = document.getElementById("form-name");
	var mail = document.getElementById("form-mail");
	var subject = document.getElementById("form-subject");
	Cookies.set('contact-dialog-name',name.value, { path: '/' });
	Cookies.set('contact-dialog-email',mail.value, { path: '/' });
	Cookies.set('contact-dialog-subject',subject.value, { path: '/' })
}

function toggleDialog() {
	var x = document.getElementById("contact-dialog");
	var y = document.getElementById("contact-us");
	x.style.display = (x.style.display === "none" ? "block" : "none");
	y.style.display = (y.style.display === "none" ? "block" : "none");
	var temp = x.style.display !== "none";
	Cookies.set('contact-dialog-shown', temp, { path: '/' });
}
