/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#include <nemesi/bufferpool.h>

/*!
 * \brief Rimuove uno slot dalla coda del Buffer di Plaout di Rete.
 *
 * Si occupa di chiamare la funzione \c podel per la cancellazione dell'elemento dalla coda di playout
 * e la funzione \c bpfree per l'eliminazione dal vettore del Bufferpool.
 *
 * \param bp puntatore al vettore del Buffer Pool corrente
 * \param po puntatore alla lista del Buffer di Playout.
 * \param index indice dell'elemento da rimuovere.
 * \return 0
 * \see podel
 * \see bpfree
 * \see bufferpool.h
 * */
int bprmv(buffer_pool * bp, playout_buff * po, int index)
{
    podel(po, index);
    bpfree(bp, index);
    pthread_cond_signal(&(bp->cond_full));

    return 0;
}
