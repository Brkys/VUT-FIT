<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class criminal extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'Kriminalni_cinnost';
    protected $primaryKey = 'ID_Cinnosti';
    public $timestamps = false;
}