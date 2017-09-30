#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position;
};

static struct RLstat RL = {
	.moving = 0,
	.speed = 5,
	.direction = 0,
	.position = 0,
};


void timer_ISR(void)
{//COMPLETAR: cada vez que el TIMER0 interrumpe el led rotante se mueve
		if(RL.direction==1){// si el giro es horario position se incrementa si su valor es <5 si no escribe un 0
			if(RL.position < 5){
				RL.position++;
			}
			else
				RL.position =0;

	}
	else if(RL.direction ==0){// si el giro es antihorario position se decrementa si su valor es >0 si no se escribe un 5
		if(RL.position > 0)
			RL.position--;
		else
			RL.position=5;

	}





	D8Led_segment(RL.position);
}

void button_ISR(void)
{
	unsigned int whichint = rEXTINTPND;
	unsigned int buttons = (whichint >> 2) & 0x3;
	//COMPLETAR: BUT1 cambia el estado de LED1 y cambia dirección de
	//movimiento del led rotante
	if(buttons == BUT1){
		led1_switch();
		if(RL.direction == 0){
			RL.direction = 1;
		}
		else {
			RL.direction = 0;
		}
	}

	//COMPLETAR: BUT2 cambia el estado de LED2
	//activa movimiento de led rotante si parado
	//o lo para si está en movimiento (actuar sobre rutinas del TIMER)

	else if(buttons == BUT2){
		led2_switch();
				if(RL.moving == 0){
					RL.moving = 1;
					tmr_update(0);
					tmr_start(TIMER0);
				}
				else {
					RL.moving = 0;
					tmr_stop(TIMER0);
				}

	}


	// eliminamos rebotes
	Delay(2000);
	// borramos el flag en extintpnd


	if(rEXTINTPND & 0x4){

		rEXTINTPND |=  (0x1 << 2); // la línea EXINT6 del controlador de interrupciones

	}
	else if(rEXTINTPND & 0x8){

		rEXTINTPND |=  (0x1 << 3); // la línea EXINT7 del controlador de interrupciones
	}
	//COMPLETAR: debemos borrar las peticiones de interrupción en
		         //EXTINTPND correspondientes a los pulsadores pulsados

}



int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas */


	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.
	portG_conf(6,EINT);
	portG_conf(7,EINT);
	portG_eint_trig(6,FALLING);
	portG_eint_trig(7,FALLING);
	portG_conf_pup(6,ENABLE);
	portG_conf_pup(7,ENABLE);



	/********************************************************************/

	/* Configuracion del timer */

	//COMPLETAR: utilizando el interfaz para los timers definido en timer.h
	//configurar el timer 0: 
	tmr_set_prescaler(0, 255);	//      valor de prescalado a 255
	tmr_set_divider(0, 2);		//      valor del divisor 1/8
	tmr_set_count(0, 62500, 62495);	//      valor de cuenta 62500 y cualquier valor de comparacion entre 1 y 62499
	tmr_update(0);				//      actualizar el contador con estos valores (update)
	tmr_set_mode(0,1);			//      poner el contador en modo RELOAD
	tmr_stop(0);				//      dejar el contador parado


	if (RL.moving)
		tmr_start(TIMER0);
	/***************************/


	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo no vectorizado
	 * y registramos una ISR para la línea IRQ
	 * Configuramos el timer 0 en modo IRQ y habilitamos
	 * esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la
	 * habilitamos
	 */

	ic_init();
	//COMPLETAR: utilizando el interfaz definido en intcontroller.h
	ic_conf_irq(1, 1);			//		habilitar la línea IRQ en modo no vectorizado
	ic_conf_fiq(0);				//		deshabilitar la línea FIQ
	ic_conf_line(13, 0);		//		configurar la línea INT_TIMER0 en modo IRQ  pag 25
	ic_conf_line(21, 0);		//		configurar la línea INT_EINT4567 en modo IRQ
	ic_enable(13);				//		habilitar la línea INT_TIMER0
	ic_enable(21);				//		habilitar la línea INT_EINT4567


	/***************************************************/

	Delay(0);
	return 0;
}



int main(void)
{
	setup();

	while (1) {
	}

	return 0;
}
