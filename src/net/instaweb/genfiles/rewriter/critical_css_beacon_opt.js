(function(){var f=encodeURIComponent,g=window,h="length",k="",l="&",m="&cs=",n="&n=",q=",",r="?",s="Content-Type",t="Microsoft.XMLHTTP",u="Msxml2.XMLHTTP",v="POST",w="application/x-www-form-urlencoded",x="load",y="oh=",z="on",A="url=",B=function(a,b,c){if(a.addEventListener)a.addEventListener(b,c,!1);else if(a.attachEvent)a.attachEvent(z+b,c);else{var d=a[z+b];a[z+b]=function(){c.call(this);d&&d.call(this)}}};g.pagespeed=g.pagespeed||{};var C=g.pagespeed;C.a=function(a){for(var b=[],c=0;c<a[h];++c)try{0<document.querySelectorAll(a[c])[h]&&b.push(a[c])}catch(d){}return b};C.computeCriticalSelectors=C.a;var D=function(a,b,c,d,e){this.b=a;this.c=b;this.e=c;this.d=d;this.f=e};
D.prototype.g=function(){for(var a=C.a(this.f),b=y+this.e+n+this.d,b=b+m,c=0;c<a[h];++c){var d=0<c?q:k,d=d+f(a[c]);if(131072<b[h]+d[h])break;b+=d}C.criticalCssBeaconData=b;var a=this.b,c=this.c,e;if(g.XMLHttpRequest)e=new XMLHttpRequest;else if(g.ActiveXObject)try{e=new ActiveXObject(u)}catch(p){try{e=new ActiveXObject(t)}catch(E){}}e&&(e.open(v,a+(-1==a.indexOf(r)?r:l)+A+f(c)),e.setRequestHeader(s,w),e.send(b))};
C.h=function(a,b,c,d,e){if(document.querySelectorAll){var p=new D(a,b,c,d,e);B(g,x,function(){g.setTimeout(function(){p.g()},0)})}};C.criticalCssBeaconInit=C.h;})();