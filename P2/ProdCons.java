import monitor.*;

class Buffer extends AbstractMonitor {
	private int slots, cont = 0;
	private double[] buffer;
	
	private Condition lecturas = makeCondition();
	private Condition huecos = makeCondition();
	
	public Buffer(int max) {
		slots = max;
		buffer = new double[slots];
	}
	
	public double leer() {
		enter();
		
		// Esperamos si el buffer está vacío
		while (cont == 0) {
			lecturas.await();
		}
		
		cont -= 1;
		double dato = buffer[cont];
		
		// Ahora hay nuevo espacio en el buffer
		huecos.signal();
		
		leave();
		
		return dato;
	}
	
	public void escribir(double dato) {
		enter();
		
		// Esperamos si el buffer está lleno
		while (cont == slots) {
			huecos.await();
		}
		
		buffer[cont] = dato;
		
		cont += 1;
		
		// Ahora hay lecturas pendientes
		lecturas.signal();
		
		leave();
	}
}

class Productor implements Runnable {
	private Buffer buf;
	private int veces;
	private int num_prod;
	Thread thr;
	
	public Productor(Buffer b, int v, int n) {
		buf = b;
		veces = v;
		num_prod = n;
		thr = new Thread(this, "productor " + num_prod);
	}
	
	public void run() {
		try {
			double item = 100 * num_prod;
			
			for (int i = 0; i < veces; i++) {
				System.out.println(thr.getName() + ", escribiendo " + item);
        		buf.escribir(item++);
			}
		} catch (Exception e) {
			System.err.println("Excepción: " + e);
		}
	}
}

class Consumidor implements Runnable {
	private Buffer buf;
	private int veces;
	private int num_cons;
	Thread thr;
	
	public Consumidor(Buffer b, int v, int n) {
		buf = b;
		veces = v;
		num_cons = n;
		thr = new Thread(this, "consumidor " + num_cons);
	}
	
	public void run() {
		try {
			for (int i = 0; i < veces; i++) {
				double item = buf.leer();
				
				System.out.println(thr.getName() + ", leído " + item);
			}
		} catch (Exception e) {
			System.err.println("Excepción: " + e);
		}
	}
}

class EjemploProdCons {
	public static void main(String args[]) {
		// Comprobación de argumentos
		if (args.length != 5) {
			System.err.println("Uso: ncons nprod tambuf niterp niterc");
			return;
		}
		
		
		// Leer parametros, crear vectores y buffer intermedio
		Consumidor[] cons = new Consumidor[Integer.parseInt(args[0])];
		Productor[]  prod = new Productor[Integer.parseInt(args[1])];
		Buffer buffer = new Buffer(Integer.parseInt(args[2]));
		
		int iter_cons = Integer.parseInt(args[3]);
		int iter_prod = Integer.parseInt(args[4]);
		
		if (cons.length * iter_cons != prod.length * iter_prod) {
			System.err.println("no coinciden número de items a producir con a cosumir");
			return;
		}    
		
		// Crear hebras
		for (int i = 0; i < cons.length; i++) 
			cons[i] = new Consumidor(buffer, iter_cons, i);
		
		for (int i = 0; i < prod.length; i++)
			prod[i] = new Productor(buffer, iter_prod, i);
		
		// Poner en marcha las hebras
		for (int i = 0; i < prod.length; i++) 
			prod[i].thr.start();
		
		for (int i = 0; i < cons.length; i++) 
			cons[i].thr.start();
	}
}