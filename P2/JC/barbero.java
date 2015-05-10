import monitor.* ;
import java.util.Random; 
// José Carlos Entrena
class Barberia extends AbstractMonitor{ 
    public int num_clientes = 0; 
    public boolean ocupado = false; 
    private Condition hay_clientes = makeCondition(); 

    // invocado por los clientes para cortarse el pelo
    public void cortarPelo (){ 
        enter(); 
        num_clientes++; 
        hay_clientes.signal(); 
        leave(); 
    }
    // invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
    public void siguienteCliente (){ 
        enter(); 
        while(num_clientes == 0)
            hay_clientes.await(); 
        leave(); 
    }
    // invocado por el barbero para indicar que ha terminado de cortar el pelo
    public void finCliente (){ 
        enter(); 
        num_clientes--; 
        leave(); 
    }
}


class Cliente implements Runnable{ 
    public Thread thr;
    public Barberia barberia; 
    public int n_cliente; 

    public Cliente(Barberia barberia, int n_cliente){
        this.barberia = barberia; 
        this.n_cliente = n_cliente; 
        thr = new Thread(this, "Cliente " + n_cliente); 
    }

    public void run (){ 
        while (true) {
            barberia.cortarPelo(); // el cliente espera (si procede) y se corta el pelo
            System.out.println("Me han cortado el pelo - Cliente " + n_cliente); 
            aux.dormir_max( 2000 ); // el cliente está fuera de la barberia un tiempo
        }
    }
}


class Barbero implements Runnable{ 
    public Barberia barberia; 
    public Thread thr;

    public Barbero(Barberia barberia){
        this.barberia = barberia; 
        thr = new Thread(this, "Barbero"); 
    }
    
    public void run (){ 
        while (true) {
            barberia.siguienteCliente ();
            aux.dormir_max( 2500 ); // el barbero está cortando el pelo
            barberia.finCliente ();
        }
    }
}

class aux{
    static Random genAlea = new Random();
    static void dormir_max( int milisecsMax ){
        try{ 
            Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
        }
        catch( InterruptedException e ){ 
            System.err.println("sleep interumpido en ’aux.dormir_max()’");
        }
    }
}

class Principal{
    public static void main( String[] args ){ 
       
        Barberia barberia = new Barberia(); 
        Barbero barbero = new Barbero(barberia); 
        barbero.thr.start(); 
      
        Cliente[] cliente = new Cliente[3]; 

        for (int i = 0; i < 3; i++){
            cliente[i] = new Cliente(barberia, i); 
            cliente[i].thr.start(); 
        }
    }  
}


