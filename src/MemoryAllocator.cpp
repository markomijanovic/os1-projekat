//
// Created by student on 8. 6. 2026..
//

#include "../h/MemoryAllocator.hpp"

// Inicijalizacija statičkih promenljivih
MemoryAllocator::MCB *MemoryAllocator::free = nullptr;
uint64 MemoryAllocator::heap_start_aligned = 0;
uint64 MemoryAllocator::heap_end_aligned = 0;


int MemoryAllocator::size_to_blocks(size_t size)
{
    size_t blocks = size / MEM_BLOCK_SIZE;
    if (size % MEM_BLOCK_SIZE)
    {
        blocks++;
    }
    return blocks;
}

void MemoryAllocator::initialize_memory()
{
    // 1. Poravnanje početne i krajnje adrese na cele blokove
    heap_start_aligned = (uint64) HEAP_START_ADDR - ((uint64) HEAP_START_ADDR % MEM_BLOCK_SIZE) + MEM_BLOCK_SIZE;
    heap_end_aligned = (uint64) HEAP_END_ADDR - ((uint64) HEAP_END_ADDR % MEM_BLOCK_SIZE);


    // 2. Postavljamo glavu liste slobodnih (free) na sam početak poravnatog heap-a
    free = (MCB *) heap_start_aligned;

    //Inicijalizujemo zaglavlje ovog slobodnog MCB prvog ogromnog
    free->isFree = true;
    free->left = 0;
    free->next = 0;
    free->prev = 0;

    free->size = (uint64) ((heap_end_aligned - heap_start_aligned) / MEM_BLOCK_SIZE);
}

void *MemoryAllocator::mem_alloc(size_t size)
{
    //Dodajemo blok na trazenu velicinu za nase zaglavlje
    size++;

    //Sada trazimo prvi dovoljno velik blok(FIRST FIT)
    MCB* current=free;
    while (current != nullptr && current->size < size)
    {
        current = current->next;
    }
    // Ako nismo našli dovoljno veliki blok, vraćamo nullptr (nema više memorije)
    if (current == nullptr)
    {
        return nullptr;
    }
    // 3. Proveravamo da li treba cepati blok na dva dela
    // Ako bi nakon cepanja ostao samo 1 blok ili manje, ne isplati se cepati
    // (zbog fragmentacije, a i taj jedan blok bi bio samo zaglavlje bez prostora za podatke).
    bool no_remains=false;
    if (current->size - size <=1)
    {
        size = current->size;// Dajemo mu ceo blok
        no_remains=true;
    }

    MCB* allocated = current;
    // 4. Sklanjanje alociranog bloka iz ulančane liste slobodnih
    if (!no_remains)
    {
        // Cepanje bloka: pravimo novi slobodan blok na adresi odmah nakon alociranog dela
        MCB* new_free_block = (MCB *) ((uint64)current + size * MEM_BLOCK_SIZE);

        new_free_block->next= allocated->next;
        new_free_block->prev = allocated->prev;
        new_free_block->size = allocated->size-size;
        new_free_block->left = allocated;
        new_free_block->isFree = true;

        if (new_free_block->prev)
        {
            new_free_block->prev->next = new_free_block;
        }else
        {
            free = new_free_block;
        }

        if (new_free_block->next)
        {
            new_free_block->next->prev = new_free_block;
        }
    }else
    {
        // Nema cepanja bloka, samo izbacujemo 'allocated' iz liste slobodnih
        if (allocated->prev)
        {
            allocated->prev->next = allocated->next;
        }else
        {
            free = allocated->next;
        }
        if (allocated->next)
        {
            allocated->next->prev = allocated->prev;
        }
    }
    //Zakljucavanje alociranog bloka
    allocated->next=0;
    allocated->prev=0;
    allocated->size=size;
    allocated->isFree=false;

    // 6. Vraćamo adresu IZA zaglavlja (korisnik ne sme da prepisuje naše zaglavlje!)
    return (void *) ((uint64)allocated + MEM_BLOCK_SIZE);
}


MemoryAllocator::MCB *MemoryAllocator::freeMCBs_join(MCB *left, MCB *right)
{// 1. Spajamo desni blok u levi
    left->size+=right->size;


    // Levi blok preuzima mesto desnog bloka u ulančanoj listi slobodnih
    left->next=right->next;
    left->prev=right->prev;
    left->isFree=true;


    // 2. Ažuriramo blok koji se nalazi fizički odmah iza desnog bloka
    MCB* rightright = (MCB*)((uint64)right+right->size*MEM_BLOCK_SIZE);

    //Proveravamo da li rightright uopste pripada nasem heap-u
    if (!((uint64)rightright<heap_end_aligned && (uint64)rightright>=heap_start_aligned))
    {
        rightright=nullptr;
    }

    //Ako postoji kazemo da mu je novi levi komsija sada nas spojeni levi
    if (rightright)
    {
        rightright->left=left;
    }

    //3.Azuriramo listu slobodnih

    if (right->prev)
    {
        right->prev->next=left;
    }else
    {
        free=left;
    }
    if (right->next)
    {
        right->next->prev=left;
    }

    return left;
}

int MemoryAllocator::mem_free(void *ptr)
{
    if (ptr==nullptr || !((uint64)ptr>=heap_start_aligned && (uint64)ptr<heap_end_aligned))
    {
        return -1;
    }
    //Vracamo se na MCB zaglavlje bloka kojeg brisemo
    MCB* to_delete = (MCB*)((uint64)ptr - MEM_BLOCK_SIZE);

    if (to_delete->isFree) return -1;//ako neko pokusa osloboditi vec slobodan prostor vracamo gersku


    // Prilazimo fizičkom levom i fizičkom desnom bloku
    MCB* left = to_delete->left;
    MCB* right= (MCB*)((uint64)to_delete+ to_delete->size*MEM_BLOCK_SIZE);

    //Provjera da desni nije van opsega slucajno
    if ((uint64)right<heap_end_aligned || (uint64)right>=heap_start_aligned)
    {
        right=nullptr;
    }

    // --- POKUŠAJ SPAJANJA (MERGING) ---

    // Slucaj 1: I levi i desni blok su već slobodni. Guraćemo sve u levi blok.
    if (left && right && left->isFree && right->isFree)
    {// Prvo moramo ručno da iščupamo 'right' iz liste slobodnih
        if (right->next) right->next->prev=right->prev;

        if (right->prev) right->prev->next=right->next;
        else free=right->next;

        // Sad levi blok guta i veličinu bloka koji brišemo, i veličinu desnog bloka
        left->size += right->size + to_delete->size;

        // Ažuriramo onog koji sledi iza desnog
        MCB* rightright=(MCB*)((uint64)right+right->size*MEM_BLOCK_SIZE);
        if (!((uint64)rightright<heap_end_aligned && (uint64)rightright>=heap_start_aligned))
        {
            rightright=nullptr;
        }
        if (rightright)
        {
            rightright->left=left;
        }
        return 0;
    }
    // Slucaj 2: Samo je desni blok slobodan
    else if (right && right->isFree)
    {
        to_delete->isFree=true;
        to_delete=freeMCBs_join(to_delete,right);// Naš blok upija desni blok i nasleđuje njegovo mesto u listi
        return 0;
    }
    // Slucaj 3: Samo je levi blok slobodan
    else if (left && left->isFree)
    {
        left->size+=to_delete->size;// Levi blok nas prosto usisa
        // Moramo obavestiti desni blok ko mu je sad levi komšija
        if (right)
        {
            right->left=left;
        }
        return 0;
    }

    // Slucaj 4: Nijedan komšija nije slobodan.
    // Samo proglašavamo naš blok slobodnim i ubacujemo ga na početak liste slobodnih.
    to_delete->isFree=true;
    to_delete->prev=nullptr;
    to_delete->next=free;

    if (free)
    {
        free->prev=to_delete;
    }
    free=to_delete;// Naš blok postaje nova glava liste

    return 0; // Uspeh
}

