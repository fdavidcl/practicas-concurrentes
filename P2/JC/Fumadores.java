import monitor.* ;
import java.util.Random;

class Estanco extends AbstractMonitor{ 
    
    public int mostrador = -1; 
    boolean hay_elem = false; 
    private Condition tabaco = makeCondition(); 
    private Condition cerillas = makeCondition(); 
    private Condition papel = makeCondition(); 
    private Condition poner = makeCondition(); 
      

    // Invocado por cada fumador, indicando su ingrediente o numero
    public void obtenerIngrediente( int miIngrediente ){     
	enter(); 
	System.out.println("Entro a coger"); 
	while (mostrador != miIngrediente) { 
            switch(miIngrediente) {
                case 0: papel.await(); 
                    System.out.println("Cojo Papel");
                    break; 
                case 1: tabaco.await(); 
                    System.out.println("Cojo Tabaco");
                    break; 
                case 2: cerillas.await(); 
                    System.out.println("Cojo Cerillas");
                    break; 
           } 
        }
         
        poner.signal(); 
        hay_elem = false;
        leave(); 
    }
    // Invocado por el estanquero, indicando el ingrediente que pone
    public void ponerIngrediente( int ingrediente ){ 
        enter();  
        while (hay_elem) 
            poner.await();
        mostrador = ingrediente; 
        hay_elem = true; 
        switch(ingrediente) {
            case 0: System.out.println("Pongo papel");
                papel.signal(); 
                break; 
            case 1: System.out.println("Pongo tabaco");
                tabaco.signal(); 
                break; 
            case 2: System.out.println("Pongo cerillas");
                cerillas.signal();                 
                break; 
        }
        leave(); 
    }
    // Invocado por el estanquero
    public void esperarRecogidaIngrediente(){ 
        enter(); 
        while (hay_elem)
            poner.await(); 
        leave(); 
    }
}

class Fumador implements Runnable{
    int miIngrediente;
    public Thread thr;
    public Estanco estanco; 
    static Random genAlea = new Random(); 

    public Fumador( int p_miIngrediente, Estanco estanco){ 
        miIngrediente = p_miIngrediente;
        this.estanco = estanco; 
        thr = new Thread(this, "Productor "+p_miIngrediente); 
    }
    
    public void run(){  
        while ( true ){ 
            estanco.obtenerIngrediente( miIngrediente );
            try{
                thr.sleep( genAlea.nextInt(2000) );
            } catch(InterruptedException e){
                System.err.println("Interrumpido al dormir"); 
            }
        }
    }
}

class Estanquero implements Runnable{ 
    public Thread thr;
    public Estanco estanco; 

    public Estanquero(Estanco estanco){
        this.estanco = estanco; 
        thr = new Thread(this, "Estanquero"); 
    }
    
    public void run(){ 
        int ingrediente ;
        while (true){
            ingrediente = (int) (Math.random () * 3.0); // 0: papel. 1: tabaco. 2: cerillas. 
            estanco.ponerIngrediente( ingrediente );
            estanco.esperarRecogidaIngrediente() ;
        }
    }
}


class Mesa{
    public static void main(String args[]){
        try{ 
            Estanco estanco = new Estanco(); 
            Estanquero pepe = new Estanquero(estanco); 
            pepe.thr.start(); 

            Fumador[] fum = new Fumador[3]; 
            
            for (int i = 0; i < 3; i++){
                fum[i] = new Fumador(i, estanco); 
                fum[i].thr.start(); 
            }
            
            pepe.thr.join(); fum[0].thr.join(); fum[1].thr.join(); fum[2].thr.join(); 

        }
        catch (Exception e){ 
            System.out.println ("Excepción.");
        }
    }
}
