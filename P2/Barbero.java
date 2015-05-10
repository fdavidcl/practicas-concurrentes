import monitor.*;
import java.util.Random;

/* Comentado porque es redefinición del archivo Fumadores.java
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
*/


class Barberia extends AbstractMonitor {
	private int ingr_actual = -1;
	
	private Condition esperando = makeCondition();
	private Condition durmiendo = makeCondition();
	private Condition cortando  = makeCondition();
	
	// invocado por los clientes para cortarse el pelo
	public void cortarPelo() {
		enter();
		
		if (!durmiendo.isEmpty()) {
			durmiendo.signal();
		} else {
			while (!cortando.isEmpty())
				esperando.await();
		}
		
		cortando.await();
		
		leave();
	}
	
	// invocado por el barbero para indicar que ha terminado de cortar el pelo
	public void finCliente() {
		enter();
		
		cortando.signal();
		
		leave();
	}
	
	// invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
	public void siguienteCliente() {
		enter();
		
		if (esperando.isEmpty()) {
			durmiendo.await();
		}
		
		esperando.signal();
		
		leave();
	}
}

class Cliente implements Runnable {
	int index;
	public Thread thr;
	Barberia sala;
	
	public Cliente(int i, Barberia e) {
		index = i;
		sala = e;
		thr = new Thread(this, "Cliente " + index);
	}
	
	public void run() {
		while (true) {
			System.out.println("Cliente " + index + ": Voy a cortarme el pelo");
			sala.cortarPelo();
			aux.dormir_max(2000);
		}
	}
}

class Barbero implements Runnable {
	public Thread thr;
	Barberia sala;
	
	public Barbero(Barberia e) {
		sala = e;
		thr = new Thread(this, "Barbero");
	}
	
	public void run() {
		while (true) {
			System.out.println("Barbero: siguiente!");
			sala.siguienteCliente();
			aux.dormir_max(2500);
			System.out.println("Barbero: listo!");
			sala.finCliente();
		}
	}
}

class EjemploBarbero {
	public static void main(String[] args) {
		if (args.length != 1) {
			System.err.println("Uso: num_clientes");
			return;
		}
		
		Barberia sala = new Barberia();
		
		Cliente[] cls = new Cliente[Integer.parseInt(args[0])];
		
		for (int i = 0; i < cls.length; i++) {
			cls[i] = new Cliente(i, sala);
		}
		
		Barbero ero = new Barbero(sala);
		
		// Poner en marcha las hebras
		for (int i = 0; i < cls.length; i++) 
			cls[i].thr.start();
		
		ero.thr.start();
	}
}