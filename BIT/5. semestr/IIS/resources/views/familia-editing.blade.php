@extends('layouts.app')

@section('title', 'Moje familie')

@section('navpanel')
	@parent
@endsection

@section('content')
<div style="padding-top: 100px"></div>
<div class="container" style="width: 80% !important; background-color: white !important;"> 
	<h1>{{ $familiaName }}</h1>
	<h4 style="padding-top: 20px;">Členové</h4>
    <input class="form-control" id="myInput" type="text" placeholder="Vyhledat člena.." onkeyup="filter()">
    <table class="table table-hover table-dark text-centered" id="filterTable">
        <thead>
            <tr>
                <th scope="col" style="width: 40%;">Jméno člena</th>
                <th scope="col" style="width: 20%;">Vyhodit</th>
                <th scope="col" style="width: 20%;"></th>
                <th scope="col" style="width: 20%;"></th>
            </tr>
        </thead>
        <tbody>
          @if(!empty($users))
            @foreach($users as $user)
              @if($user->permission !== 4)
                <tr>
                    <td>{{$user->full_name}} (
                      @if ($user->permission === 3)
                        Consigliare
                      @elseif ($user->permission === 2)
                        Caporegime - vyšší
                      @elseif ($user->permission === 1)
                        Caporegime - nižší
                      @elseif ($user->permission === 0)
                        Člen
                      @else
                        Uživatel bez rodiny
                      @endif
                      )
                    </td>
                    <td>
                      <form action="kick" method="post">
                        @csrf
                        <input type="hidden" value="{{$user->id}}" name="id">
                        <button type="submit" class="btn btn-danger" name="kick">Vyhodit</button>
                      </form>
                    </td>
                    <td>
                      @if($user->permission !== 3)
                      <form action="promote" method="post">
                        @csrf
                        <input type="hidden" value="{{$user->id}}" name="id">
                        <button type="submit" class="btn btn-primary" name="promote">Povýšit</button>
                      </form>
                      @endif
                    </td>
                    <td>
                      @if($user->permission != 0)
                      <form action="degrade" method="post">
                        @csrf
                        <input type="hidden" value="{{$user->id}}" name="id">
                        <button type="submit" class="btn btn-secondary" name="degrade">Degradovat</button>
                      </form>
                      @endif
                    </td>
                </tr>
              @endif
            @endforeach
          @else
            <tr>
              <td colspan="3">Nenalezení uživatelé</td>
            </tr>
          @endif
        </tbody>
    </table>
   @if(!empty($alianceRequests))
    <h4 style="padding-top: 20px;">Aliance</h4>
    <div style="padding-top: 20px"></div>
    <table class="table table-hover table-dark text-centered">
      <thead>
          <tr>
              <th scope="col" style="width: 60%;">Pozvánka od</th>
              <th scope="col" style="width: 20%;"></th>
              <th scope="col" style="width: 20%;"></th>
          </tr>
      </thead>
      <tbody>
        @foreach($alianceRequests as $alianceRequest)
          <tr>
            <td>
              {{ $alianceRequest->JmenoFamilie }}
            </td>
            <td>
              <form action="accept-aliance" method="post">
                @csrf
                <input type="hidden" value="{{ $alianceRequest->PozvankaOd }}" name="ID_Familie">
                <button type="submit" class="btn btn-primary" name="accept-aliance">Přijmout</button>
              </form>
            </td>
            <td>
              <form action="decline-aliance" method="post">
                @csrf
                <input type="hidden" value="{{ $alianceRequest->PozvankaOd }}" name="ID_Familie">
                <button type="submit" class="btn btn-danger" name="decline-aliance">Odmítnout</button>
              </form>
            </td>
          </tr>
        @endforeach
      </tbody>
    </table>
    @endif
    <div style="padding-top: 20px"></div>
</div>

<script>
function filter() {
  // Declare variables 
  var input, filter, table, tr, td, i, txtValue;
  input = document.getElementById("myInput");
  filter = input.value.toUpperCase();
  table = document.getElementById("filterTable");
  tr = table.getElementsByTagName("tr");
  for (i = 0; i < tr.length; i++) {
    td = tr[i].getElementsByTagName("td")[0];
    if (td) {
      txtValue = td.textContent || td.innerText;
      if (txtValue.toUpperCase().indexOf(filter) > -1) {
        tr[i].style.display = "";
      } else {
        tr[i].style.display = "none";
      }
    } 
  }
}
</script>
@endsection