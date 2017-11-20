/*
 * web.h
 *
 *  Created on: 17/11/2017
 *      Author: root
 */

#ifndef WEB_H_
#define WEB_H_

const char HttpWebPageHeader [] =
		"<!DOCTYPE HTML>"
		"<html>"
		"<head><title>METEO-CIAA</title>"
		"<meta http-equiv=\"refresh\" content=\"15\">"
		"</head>"
		"<h1>METEO-CIAA</h1>"
		"<h1>Central meteorol&oacute;gica EDU-CIAA</h1><hr />"
		"<p>Aqui se visualizan los datos obtenidos en forma remota de la central meteorol&oacute;gica basada en la placa <strong>EDU-CIAA</strong> desarrollada para el curso de Sistemas Embebidos del <strong>INET</strong></p>"
		"<table class=\"editorDemoTable\" style=\"height: 150px; width: 400px; background-color: #d4f8ff;\" border=\"3\">"
		;

//<tr><td><strong>Magnitud<br /></strong></td><td><strong>Valor</strong></td><td><strong>Unidad</strong></td></tr>
//<tr><td>Temperatura</td><td>111.1</td><td>&deg;C</td></tr>
//<tr><td>Humedad</td><td>222.2</td><td>%RH</td></tr>
//<tr><td>Velocidad del viento</td><td>333.3</td><td>Km/h</td></tr>

const char HttpWebPageEnd [] =
		"</tbody></table><hr /><p>Alumno: Leonardo Davico</p>"
		"</html>";

char HttpWebPageBody [1501];

#endif /* WEB_H_ */
