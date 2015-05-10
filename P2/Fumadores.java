import monitor.*;
import java.util.Random;

class aux {
	static Random genAlea = new Random();
	
	static void dormir_max(int milisecsMax) { 
		try { 
			Thread.sleep(genAlea.nextInt(milisecsMax));
		}
		catch (InterruptedException e) { 
			System.err.println("sleep interumpido en ’aux.dormir_max()’");
		}
	}
}

class Estanco extends AbstractMonitor {
	private int ingr_actual = -1;
	
	private Condition[] ingredientes;
	private Condition estanco_libre = makeCondition();
	
	public Estanco() {
		ingredientes = new Condition[3];
		
		for (int i = 0; i < ingredientes.length; i++) {
			ingredientes[i] = makeCondition();
		}
	}
	
	// invocado por cada fumador, indicando su ingrediente o numero
	public void obtenerIngrediente( int miIngrediente ) {
		enter();
		
		if (ingr_actual != miIngrediente) {
			ingredientes[miIngrediente].await();
		}
		
		// recogemos ingrediente
		System.out.println("Estanco: se recoge " + ingr_actual);
		ingr_actual = -1;
		
		estanco_libre.signal();
		
		leave();
	}
	
	// invocado por el estanquero, indicando el ingrediente que pone
	public void ponerIngrediente( int ingrediente ) {
		enter();
		
		ingr_actual = ingrediente;
		System.out.println("Estanco: colocamos " + ingr_actual);
		ingredientes[ingr_actual].signal();
		
		leave();
	}
	
	// invocado por el estanquero
	public void esperarRecogidaIngrediente() {
		enter();
		
		if (ingr_actual > -1) {
			System.out.println("Estanco: se espera la recogida");
			estanco_libre.await();
		}
		
		leave();
	}
}

class Fumador implements Runnable {
	int miIngrediente;
	public Thread thr;
	Estanco estanco;
	
	public Fumador(int p_miIngrediente, Estanco e) {
		miIngrediente = p_miIngrediente;
		estanco = e;
		thr = new Thread(this, "fumador " + miIngrediente);
	}
	
	public void run() {
		while (true) {
			System.out.println("F" + miIngrediente + ": Esperando");
			estanco.obtenerIngrediente(miIngrediente);
			System.out.println("F" + miIngrediente + ": Fumando");
			aux.dormir_max(2000);
		}
	}
}

class Estanquero implements Runnable {
	public Thread thr;
	Estanco estanco;
	
	public Estanquero(Estanco e) {
		estanco = e;
		thr = new Thread(this, "estanquero");
	}
	
	public void run() {
		int ingrediente;
		
		while (true) {
			ingrediente = (int) (Math.random () * 3.0);
			estanco.ponerIngrediente(ingrediente);
			estanco.esperarRecogidaIngrediente();
		}
	}
}

class EjemploFumadores {
	public static void main(String[] args) {
		
		Estanco est = new Estanco();
		
		Fumador[] fums = new Fumador[3];
		
		for (int i = 0; i < fums.length; i++) {
			fums[i] = new Fumador(i, est);
		}
		
		Estanquero ero = new Estanquero(est);
		
		// Poner en marcha las hebras
		for (int i = 0; i < fums.length; i++) 
			fums[i].thr.start();
		
		ero.thr.start();
	}
}