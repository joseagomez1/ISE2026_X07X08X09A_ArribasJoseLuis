t <html>
t <head>
t <title>Monitor RTC</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
t var formUpdate = new periodicObj("rtc.cgx", 1000);
t function plotRTCGraph() {
t  hora = document.getElementById("hora_rtc").value;
t  fecha = document.getElementById("fecha_rtc").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("RTCChkBox").checked == true) {
t   updateMultiple(formUpdate,plotRTCGraph);
t   RTC_elTime = setTimeout(periodicUpdateRTC, formUpdate.period);
t  }
t  else
t   clearTimeout(RTC_elTime);
t }
t </script>
t <style>
t body{
t  background-color:#eef3f7;
t  font-family:Arial;
t }
t .rtcBox{
t  width:420px;
t  margin:auto;
t  background:#ffffff;
t  border:2px solid #2c6fa3;
t  border-radius:8px;
t  padding:20px;
t  box-shadow:0px 3px 8px rgba(0,0,0,0.2);
t }
t .title{
t  text-align:center;
t  color:#2c6fa3;
t }
t .field{
t  margin-top:15px;
t }
t .field label{
t  font-weight:bold;
t  display:block;
t }
t .field input{
t  width:100%;
t  padding:6px;
t  border:1px solid #ccc;
t  border-radius:4px;
t }
t .updateBox{
t  margin-top:20px;
t  text-align:center;
t }
t .updateBox input{
t  transform:scale(1.2);
t }
t </style>
t </head>
i pg_header.inc
t <body>
t <div class="rtcBox">
t <h2 class="title">Monitor de Hora RTC</h2>
t <p align=center>Visualización del reloj en tiempo real del sistema.</p>
t <form action=rtc.cgi method=post name=cgi>
t <input type=hidden value="lang" name=pg>
t <div class="field">
t  <label>Hora del sistema</label>
c r 1 <input type=text name=rtc1 id="hora_rtc" maxlength=20 value="%s">
t </div>
t <div class="field">
t  <label>Fecha del sistema</label>
c r 2 <input type=text name=rtc2 id="fecha_rtc" maxlength=20 value="%s">
t </div>
t <div class="updateBox">
t  Actualización automática
t  <input type="checkbox" id="RTCChkBox" onclick="periodicUpdateRTC()">
t </div>
t </form>
t </div>
t </body>
i pg_footer.inc
t </html>
.   