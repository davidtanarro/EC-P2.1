#include "44b.h"
#include "gpio.h"

/* Port B interface implementation */

int portB_conf(int pin, enum port_mode mode)
{
	// Configuramos para cada modo (En este caso hay 2) para el puerto B, como SIGOUT (Como controaldor) o como OUTPUT (Como salida).

	int ret = 0;
	if (pin < 0 || pin > 10){
		return -1;
	}
	else{
	if (mode == SIGOUT){

		rPCONB |= (0x1 << pin);  // Escribimos un 1 en la posicion pin
	}
		//COMPLETAR: configurar el puerto B para que pin funcione en modo SIGOUT
	else if (mode == OUTPUT){
		rPCONB &=~ (0x1 << pin); // Escribimos un 0 en la posicion pin

	}
		//COMPLETAR: configurar el puerto B para que pin funcione en modo OUTPUT
	else{
		ret = -1;
	}
	return ret;
	}
}

int portB_write(int pin, enum digital val)
{
	// Configuramos el registro para que encienda o apague el led.

	if (pin < 0 || pin > 10)
		return -1;

	if (val < 0 || val > 1)
		return -1;

	if (val){

		rPDATB |= (0x1 << pin); // Escribimos un 1 para apagar el led.

		//COMPLETAR: configurar para que el led (indicado por pin) se apague
	}
	else{
		rPDATB &=~ (0x1 << pin); //Escribimos un 0 para encender el led
		//COMPLETAR: configurar para que el led (indicado por pin) se encienda

	}
	return 0;
}

/* Port G interface implementation */

int portG_conf(int pin, enum port_mode mode)
{

	// Configuramos para cada modo (En este caso hay 4) para el puerto G, como SIGOUT (Como controaldor), como OUTPUT (Como salida),
	// como INPUT (Entrada) y como EINT (Interrupciones).

	int pos  = pin*2; //Multiplicamos por 2 porque utilizamos 2 bits

	if (pin < 0 || pin > 7)
		return -1;

	switch (mode) {
		case INPUT:
			// Escribimos 00 en la posicion que nos indique el pin. (Mirar la tabla pag 29)

			rPCONG &=~ (0x3 << pos); // Como es AND escribimos un 00 en la posicion que nos indique pin.
			//COMPLETAR: configurar el puerto G para que pin funcione en modo INPUT


			break;
		case OUTPUT:
			rPCONG &=~ (0x3 << pos); // Como es AND escribimos un 00 en la posicion que nos indique pin.
			rPCONG |= (0x1 << pos);  // Como es OR escribimos un 01 en la posicion que nos indique pin.

			//COMPLETAR: configurar el puerto G para que pin funcione en modo OUTPUT


			break;
		case SIGOUT:
			rPCONG &=~ (0x3 << pos); // Como es AND escribimos un 00 en la posicion que nos indique pin.
			rPCONG |= (0x2 << pos);  // Como es OR escribimos un 10 en la posicion que nos indique pin.

			//COMPLETAR: configurar el puerto G para que pin funcione en modo SIGOUT


			break;
		case EINT:
			//rPCONG &=~ (0x3 << pos);//sobra?
			rPCONG |= (0x3 << pos); // Como es OR escribimos un 11 en la posicion que nos indique pin.

			//COMPLETAR: configurar el puerto G para que pin funcione en modo EINT


			break;
		default:
			return -1;
	}

	return 0;
}

int portG_eint_trig(int pin, enum trigger trig)
{
	// Configuramos el tipo de interrupción (nivel alto/bajo, flanco subida/bajada/ambos) mediante el registro EXTINT del puerto.

	int pos = pin*4; //Multiplicamos por 4 porque necesitamos 3 bits

	if (pin < 0 || pin > 7)
		return -1;

	switch (trig) {
		case LLOW:
			rEXTINT &=~ (0x7 << pos);//000

			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por nivel bajo


			break;
		case LHIGH:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x1 << pos);//001
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por nivel alto


			break;
		case FALLING:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x2 << pos);//01x
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por flanco de bajada


			break;
		case RISING:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x4 << pos); //10x
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por flanco de subida


			break;
		case EDGE:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x6 << pos); //11x
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por cualquier flanco


			break;
		default:
			return -1;
	}
	return 0;
}

int portG_write(int pin, enum digital val)
{

	// Escribimos en el registro de datos.

	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1;

	if (val < 0 || val > 1)
		return -1;

	if ((rPCONG & (0x3 << pos)) != (0x1 << pos))
		return -1;

	if (val){
		rPDATG |= (0x1 << pos); //COMPLETAR: escribir en el registro de datos del puerto G (en pin) un 1
	}
	else
		rPDATG &=~ (0x1 << pos);
		//COMPLETAR: escribir en el registro de datos del puerto G (en pin) un 0


	return 0;
}

int portG_read(int pin, enum digital* val)
{
	// Leemos del registro de datos.

	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1;

	if (rPCONG & (0x3 << pos))
		return -1;

	if(rPDATG & (0x3 << pos))//COMPLETAR:true si está a 1 en rDATG el pin indicado por el parámetro pin)
		*val=HIGH;
	else
		*val=LOW;

	return 0;
}

int portG_conf_pup(int pin, enum enable st)
{
	// Controlar los rebotes de la señal. registro de configuración que permite activar (bit a ’0’) o no (bit a ’1’) una
	// resistencia de pull-up por cada pin.

	if (pin < 0 || pin > 7){
		return -1;
	}
	if (st != ENABLE && st != DISABLE){
		return -1;
	}
	//COMPLETAR: activar la resistencia de pull-up en pin
	if (st == ENABLE){
		rPUPG &=~(0x1 << pin);

	}
	else{
		rPUPG |= (0x1 << pin);
		//COMPLETAR: desactivar la resistencia de pull-up en pin
	}

	return 0;
}

