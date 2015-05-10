import monitor.*; 
// José Carlos Entrena
class Buffer extends AbstractMonitor{
    private int num_slots = 0, cont = 0; 
    private double[] buffer = null; 
    private Condition no_lleno = makeCondition();
    private Condition no_vacio = makeCondition();

    public Buffer(int slots){
        if (slots > 0)
            num_slots = slots; 
        else 
            num_slots = 1; 
        buffer = new double[num_slots]; 
    }

    public void poner(double elem){
        enter();
        while (cont >= num_slots) 
            no_lleno.await();
        cont++; 
        buffer[cont-1] = elem;
        no_vacio.signal(); 
        leave();
    }
    
    public double tomar(){ 
        enter();
        double elem; 
        while (cont == 0) 
            no_vacio.await();
        elem = buffer[cont-1]; 
        cont--; 
        no_lleno.signal(); 

        leave();
        return elem; 
    }
}

class Productor implements Runnable{
    private Buffer buffer; 
    private int n_veces; 
    public Thread thr; 
    double prod; 

    public Productor(Buffer buffer, int p_nveces, int num_prod){ 
        this.buffer = buffer; 
        n_veces = p_nveces;
        thr = new Thread(this,"Productor " + num_prod);
        prod = (double) num_prod; 
    }

    public void run(){ 
        try{
            for(int i = 0 ; i < n_veces ; i++){ 
                System.out.println( thr.getName()+": solicita escritura.");
                buffer.poner(prod+i);
                System.out.println("Elemento colocado"); 
            }
        } catch(Exception e){
            System.err.println("Excepción"); 
        }
    }
}


class Lector implements Runnable{
    private Buffer buffer;
    private int n_veces, lector; 
    public Thread thr; 

    public Lector(Buffer buffer, int p_nveces, int num_lec){ 
        this.buffer = buffer; 
        n_veces = p_nveces;
        lector = num_lec; 
        thr = new Thread(this,"Lector " + num_lec);
    }

    public void run(){ 
        try{
            double item; 
            for(int i = 0 ; i < n_veces ; i++){ 
                System.out.println("Soy lector " + lector + " y voy a leer.");
                item = buffer.tomar(); 
                System.out.println("Lector " + lector + " lee " + item); 
            }
        } catch(Exception e){
            System.err.println("Excepción"); 
        }   
    }                
}

class Nueva{
    public static void main(String args[]){
        try{ 
            Buffer buf = new Buffer(5);  
            Productor e = new Productor(buf, 10, 1); 
            e.thr.start(); 

            Lector[] lec = new Lector[2]; 
            
            for (int i = 0; i < 2; i++){
                lec[i] = new Lector(buf, 5, i); 
                lec[i].thr.start(); 
            }

        }
        catch (Exception e){ 
            System.out.println ("Excepción.");
        }
    }
}

