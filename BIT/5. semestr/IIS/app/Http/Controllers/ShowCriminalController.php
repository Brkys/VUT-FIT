<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;

use App\CProvadiK;

class ShowCriminalController extends Controller
{
    public function show() {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else if($_SESSION['permission'] < 0 || $_SESSION['permission'] > 4){
            return redirect('no-permission');
        }
        else
        {
            $result = DB::select("SELECT CinnostFamilii.ID_Cinnosti, Adresa, JmenoFamilie, TypCinnosti, DatumZacatku, DatumUkonceni FROM Familie INNER JOIN CinnostFamilii ON Familie.ID_Familie=CinnostFamilii.ID_Familie INNER JOIN Kriminalni_cinnost ON Kriminalni_cinnost.ID_Cinnosti=CinnostFamilii.ID_Cinnosti INNER JOIN Uzemi ON Kriminalni_cinnost.ID_Uzemi=Uzemi.ID_Uzemi INNER JOIN users ON users.familia_id=Familie.ID_Familie WHERE Familie.ID_Familie=".$_SESSION['familia']." GROUP BY ID_Cinnosti, JmenoFamilie", [1]);

            return view('show-criminal')->with('result', $result);
        }
    }
}
